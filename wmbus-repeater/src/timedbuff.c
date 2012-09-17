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
#include <string.h>
#include <time.h>

#include "timedbuff.h"

#define BUF_SIZE 2048
#define NUM_OF_BUFS 32

typedef struct _timed_buffer
{
	time_t time_delta;
	char used;
	unsigned char data[BUF_SIZE];
	unsigned int data_size;
} TIMEDBUF;

static TIMEDBUF timed_buffer[NUM_OF_BUFS];

// private helper functions

static int reset_timedbuf(int idx)
{
	timed_buffer[idx].used = 0;
	timed_buffer[idx].time_delta = 0;
	timed_buffer[idx].data_size = 0;

	// not really needed:
	// memset(timed_buffer[idx].data, 0, sizeof(timed_buffer[idx].data));
	return 0;
}

static int find_first_free_slot(void)
{
	int i;
	for(i=0 ; i<NUM_OF_BUFS ; i++)
		if(0 == timed_buffer[i].used)
			return i;

	return -1;
}

// public interface

int timedbuff_init(void)
{
	int i;
	for(i=0 ; i<NUM_OF_BUFS ; i++)
		reset_timedbuf(i);
	return 0;
}

unsigned int timedbuff_store(unsigned char *buf, unsigned int bufsize, time_t time_delta)
{
	if(BUF_SIZE < bufsize) return 0;

	int slot = find_first_free_slot();
	if(-1 == slot) return 0;

	timed_buffer[slot].used = 1;
	timed_buffer[slot].time_delta = time_delta;
	timed_buffer[slot].data_size = bufsize;
	memcpy(timed_buffer[slot].data, buf, bufsize);
	return bufsize;
}

unsigned int timedbuff_retrieve(unsigned char *data, unsigned int max_datasize)
{
	int i;
	for(i=0 ; i<NUM_OF_BUFS ; i++)
	{
		unsigned int datasize = timed_buffer[i].data_size;
		if( (0==timed_buffer[i].time_delta) && (1==timed_buffer[i].used) && (max_datasize>=datasize) )
		{
			memcpy(data, timed_buffer[i].data, datasize);
			reset_timedbuf(i);
			return datasize;
		}
	}
	return 0;
}

int timedbuff_update_time(time_t time_elapsed)
{
	int i, have_data = 0;

	for(i=0 ; i<NUM_OF_BUFS ; i++)
	{
		if( timed_buffer[i].time_delta <= time_elapsed )
		{
			have_data++;
			timed_buffer[i].time_delta = 0; // ready to be retrieved
		}
		else
			timed_buffer[i].time_delta -= time_elapsed; // still needs to wait...
	}

	return have_data;
}

time_t timedbuff_get_delay(void)
{
	int i, y=1; // kludge for first hit
	time_t d = 0;

	for(i=0 ; i<NUM_OF_BUFS ; i++)
	{
		if( timed_buffer[i].used && (y || timed_buffer[i].time_delta < d) )
		{
			y = 0;
			d = timed_buffer[i].time_delta;
		}
	}

	return d;
}

