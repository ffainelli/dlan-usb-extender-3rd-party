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
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "amber.h"
#include "int2bcd.h"

void hex_dump(unsigned char *buf, int len);

/*
 * Amber configuration data
 */

static int cfg_fd = -1;
static struct termios cfg_oldtio;
unsigned char bcdSerialNo[4];

/*
 * Private utility functions
 */

static unsigned char calc_xor_checksum(unsigned char *buf, int len)
{
	int i;
	unsigned char res=0;
	for(i=0;i<len;i++)
		res ^= buf[i];
	return res;
}

static tWMBUS_MODE mapStringToWMbusMode(const char *mode)
{
	if(!strcasecmp(mode, "S1")) return mode_S1;
	if(!strcasecmp(mode, "S2")) return mode_S2;
	if(!strcasecmp(mode, "T1meter")) return mode_T1meter;
	if(!strcasecmp(mode, "T1other")) return mode_T1other;
	if(!strcasecmp(mode, "T2meter")) return mode_T2meter;
	if(!strcasecmp(mode, "T2other")) return mode_T2other;
	if(!strcasecmp(mode, "retain")) return mode_retain;

	return mode_unknown;
}

static void storeBcdSerialNo(unsigned char *buf)
{
	uint64_t tmp, serno;

	serno = (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | (buf[3]<<0);

	tmp = serno & 0x00FFFFF;
	int2bcd(6, (uint64_t*)&tmp, (uint8_t*)&bcdSerialNo[0]); 
	tmp = ((serno & 0xFF000000) >> 24);
	int2bcd(2, (uint64_t*)&tmp, (uint8_t*)&bcdSerialNo[3]);
}

/*
 * Public Interface
 */

int amber_open(const char *devname, const char *szWMbusMode)
{
	tWMBUS_MODE wmbus_mode = mapStringToWMbusMode(szWMbusMode);

	if(mode_unknown == wmbus_mode)
	{
		printf("ERROR: Unknown Wireless M-Bus mode\n");
		return -1;
	}
	printf("Using Wireless M-Bus mode %s\n", szWMbusMode);

	if(-1 != cfg_fd)
	{
		printf("ERROR: Unable to handle multiple amber devices\n");
		return -1;
	}

	cfg_fd = open(devname, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if(-1 == cfg_fd)
	{
		printf("ERROR: Can not open %s\n", devname);
		return -1;
	}

	// Save old and set new serial parameters
	tcgetattr(cfg_fd, &cfg_oldtio);

	struct termios newtio; // 9600 8N1
	memset(&newtio, 0, sizeof(newtio));

	newtio.c_cflag = CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = newtio.c_cc[VMIN] = 0;

	cfsetispeed(&newtio, B9600);
	cfsetospeed(&newtio, B9600);
	tcflush(cfg_fd, TCIFLUSH);
	tcsetattr(cfg_fd, TCSANOW, &newtio);

	// set Wireless M-Bus mode
	unsigned char buf[16];
	buf[0] = 0xff;
	buf[1] = 0x09;
	buf[2] = 0x03; // len
	buf[3] = 0x46; // cfg register
	buf[4] = 0x01; // len
	buf[5] = wmbus_mode; // mode
	if (wmbus_mode != mode_retain)
		amber_write_command(buf, 6);
	else
		printf("Not changing adapter mode\n");
	sleep(1);
	tcflush(cfg_fd, TCIFLUSH); // kill cmd reply

	// get serial number
	buf[0] = 0xff;
	buf[1] = 0x0B;
	buf[2] = 0x00;
	buf[3] = 0xF4;
	amber_write_command(buf, 4);
	sleep(1);
	amber_read(buf, sizeof(buf));
	storeBcdSerialNo(buf+3);

	tcflush(cfg_fd, TCIFLUSH);
	return 0;
}

int amber_close()
{
	if(-1 != cfg_fd)
	{
		tcsetattr(cfg_fd, TCSANOW, &cfg_oldtio);
		close(cfg_fd);
		cfg_fd = -1;
		return 0;
	}
	return -1;
}

int amber_write(unsigned char *buf, int buflen)
{
	if(-1 == cfg_fd) return -1;
	int w = write(cfg_fd, buf, buflen);
	fsync(cfg_fd);
	return w;
}

int amber_write_command(unsigned char *buf, int buflen)
{
	#define MAXBUFLEN 128

	if((-1 == cfg_fd) || (buflen+1 > MAXBUFLEN)) return -1;

	unsigned char tmpbuf[MAXBUFLEN];
	memcpy(tmpbuf, buf, buflen);
	tmpbuf[buflen] = calc_xor_checksum(tmpbuf, buflen);
	return amber_write(tmpbuf, buflen+1);
}

int amber_read(unsigned char *buf, int maxbuflen)
{
	return read(cfg_fd, buf, maxbuflen);
}

int amber_get_fd()
{
	return cfg_fd;
}

int amber_fd_is_valid(const char *device)
{
	struct stat buf;

	return stat(device, &buf);
}
