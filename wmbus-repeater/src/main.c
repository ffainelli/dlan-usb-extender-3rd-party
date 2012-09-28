/*
 * Copyright (c) 2012, devolo AG
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

#include "amber.h"
#include "wmbus.h"
#include "wmbus_dvl.h"
#include "network.h"
#include "timedbuff.h"

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

static int repeat_directly = 0;
static time_t time_delay = 0;
static unsigned char access_nr = 0;

static unsigned char get_random_number(void)
{
	unsigned char rv = 0;
	int fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);

	if(0<=fd && read(fd, &rv, sizeof(rv))==sizeof(rv))
		;

	if(0<=fd) close(fd);

	return rv;
}

static char *get_timestamp(void)
{
	static char t[9];
	time_t clock;
	clock = time(NULL);
	strncpy(t, asctime(localtime(&clock))+11, 8);
	t[8] = 0;
	return t;
}

#ifdef DEBUG
static void hex_dump(unsigned char *buf, int len)
{
	int i;
	for(i=0;i<len;i++)
		printf("0x%02x ", (int) (buf[i]));

	printf("\n");
}
#endif

static time_t send_timed_buffers(void)
{
	static time_t last_time = 0;
	time_t current_time;
	time_t time_elapsed;

	if(0 == last_time) // init once
		last_time = current_time = time(NULL);
	else
		current_time = time(NULL);
	
	if( current_time > last_time )
		time_elapsed = (time_t) difftime(current_time, last_time);
	else
	{
		// time_t is a signed 32-bit integer on this platform.
		// Since the device's time starts at 00:00:00 (after every boot), we have ~68 years
		// before a wrap-around occurs. That's time enough for our product lifetime and
		// therefore we do not handle this case.
		time_elapsed = 0;
	}

	last_time = current_time;

	if( 0 != timedbuff_update_time(time_elapsed) )
	{
		/* we have buffers to send */
		unsigned char data[2048];
		unsigned int datalen;
		while( 0 != (datalen = timedbuff_retrieve(data, sizeof(data))) )
		{
			printf("(%s) sending delayed data to wM-Bus (%d bytes):\n", get_timestamp(), datalen);
			wmbus_hex_dump(data, datalen);
			wmbus_dump(data, datalen);
			printf("\n");
			amber_write(data, datalen);
		}
	}

	return timedbuff_get_delay();
}

static int usage(const char *name)
{
	printf("Usage: %s -d <device path> <wM-Bus mode>\n", name);
	printf("           wM-Bus mode can be: S1, S2, T1meter, T1other, T2meter, T2other\n");
	return -1;
}

int main(int argc, char **argv)
{
	int opt;
	const char *appname = strdup(argv[0]);
	const char *device = NULL;
	const char *interface = NULL;
	const char *mode = NULL;

	while ((opt = getopt(argc, argv, "d:i:h")) > 0) {
		switch (opt) {
		case 'd':
			device = optarg;
			break;
		case 'i':
			interface = optarg;
			break;
		case 'h':
		default:
			usage(appname);
			break;
		}
	}

	argc -= optind;
	argv += optind;

	mode = argv[0];
	
	printf("# dLAN Wireless M-Bus Repeater\n");

	if(1 != argc)
	{
		return usage(appname);
	}

	if(-1 == amber_open(device, mode))
		return -1;

	if(-1 == netw_open(interface))
	{
		amber_close();
		return -1;
	}

	// prepare timed buffers
	timedbuff_init();

	// prepare select
	fd_set rfds;
	int nfd = max( amber_get_fd(), netw_get_fd() ) + 1;

	// main loop

	unsigned char module_data[2048];
	int len=0, offs=0;
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(amber_get_fd(), &rfds);	
		FD_SET(netw_get_fd(), &rfds);	

		struct timeval timeout;
		timeout.tv_sec = time_delay;
		timeout.tv_usec = 0;

		printf("(%s) select is sleeping for %ld secs\n", get_timestamp(), time_delay);
		int rc = select(nfd, &rfds, NULL, NULL, (timeout.tv_sec ? &timeout : NULL) );

		if (amber_fd_is_valid(device) < 0)
		{
			printf("ERROR: device disconnected\n");
			goto out;
		}

		if(-1 == rc)
		{
			printf("ERROR: select failed\n");
			continue;
		}

		// check if any timed buffers need to be sent
		time_delay = send_timed_buffers();

		if(!rc) // no data via serial/network
			continue;

		// Data from serial amber module?
		if(FD_ISSET(amber_get_fd(), &rfds))
		{
			unsigned char ibuf[2048];
			int r = amber_read(ibuf, sizeof(ibuf));
			if(0==r) continue;
			if(0==len)
			{
				if( 0 && 0xff == ibuf[0] ) // cmd feedback from module
				{
					memcpy(&module_data[0], &ibuf[0], r);
					offs = r; len = 0;
				}
				else // data from module
				{
					len = (int) ibuf[0] + 1;
					memcpy(&module_data[0], &ibuf[0], r);
					offs = r; len -= r;
				}
			}
			else
			{
				memcpy(&module_data[offs], &ibuf[0], r);
				offs += r; len -= r;
			}

			if(0==len)
			{
				int should_repeat = oms_unidir_should_repeat(module_data, offs);
				printf("(%s) wM-Bus %s (%d bytes):\n", get_timestamp(), should_repeat?"TO eth":"ignore", offs);
				wmbus_hex_dump(module_data, offs);
				wmbus_dump(module_data, offs);
				printf("\n");
				if(should_repeat)
				{
					unsigned short sw = wmbus_apl_get_signature_word(module_data);
					wmbus_set_hopcount(&sw);
					wmbus_apl_set_signature_word(module_data, sw);
					netw_send(module_data, offs);

					// now we need to check for SND-IR (C=0x46, hopcount=0) and reply with SND-NKE.
					// the telegram's hopcount was 0 initially, otherwise we would not be in the should_repeat branch.
					if(0x46 == wmbus_dll_get_c(module_data))
					{
						unsigned char sndnke[2048];
						int sndnke_len = sizeof(sndnke);
						// prepare SND-NKE telegram
						sndnke_len = wmbus_dvl_create_snd_nke(module_data, offs, sndnke, sndnke_len);
						
						// fill in details:
						// set meter id, meter manu, meter vers, meter devtype
						wmbus_apl_set_meter_id(sndnke, wmbus_dll_get_id(module_data));
						wmbus_apl_set_meter_manu(sndnke, wmbus_dll_get_manu(module_data));
						wmbus_apl_set_meter_version(sndnke, wmbus_dll_get_version(module_data));
						wmbus_apl_set_meter_devtype(sndnke, wmbus_dll_get_devtype(module_data));

						// access nr, status, sigword
						wmbus_apl_set_access_nr(sndnke, access_nr++);
						wmbus_apl_set_status(sndnke, 0x00);
						wmbus_apl_set_signature_word(sndnke, 0x0000);

						// send to netw (mark specific delay???)
						netw_send(sndnke, sndnke_len);
					
						// send to wmbus with delay [2..5]
						time_t buff_delay = (get_random_number()%3)+2; // OMS spec tells us to delay SND-NKE for 2..5 seconds
						timedbuff_store(sndnke, sndnke_len, buff_delay);
						time_delay = timedbuff_get_delay();
						printf("(%s) send SND-NKE to wM-Bus, delaying for %ld secs (%d bytes):\n", get_timestamp(), buff_delay, sndnke_len);
						wmbus_hex_dump(sndnke, sndnke_len);
						wmbus_dump(sndnke, sndnke_len);
						printf("\n");
					}
				}
			}
		}

		// Data from network?
		if(FD_ISSET(netw_get_fd(), &rfds))
		{
			unsigned char buf[2048];
			int r = netw_receive(buf, sizeof(buf));
			if(-1==r)
			{
				printf("ERROR: netw_recv\n");
			}
			else if(0<r)
			{
				if(repeat_directly)
				{
					printf("(%s) eth to wM-Bus (%d bytes):\n", get_timestamp(), r);
					wmbus_hex_dump(buf, r);
					wmbus_dump(buf, r);
					printf("\n");
					amber_write(buf, r);
				}
				else
				{
					time_t buff_delay;
					if((23 == r) && (0x40 == wmbus_dll_get_c(buf))) // special case: (our own) SND-NKE
						buff_delay = (get_random_number()%3)+2; // OMS spec tells us to delay for 2..5 seconds
					else
						buff_delay = (get_random_number()%20)+5; // OMS spec tells us to delay for 5..25 seconds
					timedbuff_store(buf, r, buff_delay);
					time_delay = timedbuff_get_delay();
					printf("(%s) rcvd from eth, delaying for %ld secs (%d bytes):\n", get_timestamp(), buff_delay, r);
					wmbus_hex_dump(buf, r);
					wmbus_dump(buf, r);
					printf("\n");
				}
			}
		}
	}

out:
	netw_close();
	amber_close();

	return 0;
}
