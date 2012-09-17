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
#include "ringbuff.h"

#define RB_BUF_SIZE 1024
#define RB_NUM_OF_BUFS 32

static unsigned char stored_data[RB_NUM_OF_BUFS][RB_BUF_SIZE];
static unsigned int stored_size[RB_NUM_OF_BUFS];
static int head = 0;
static int current = 0;

unsigned int ringbuff_store(unsigned char *buf, unsigned int bufsize)
{
	if( RB_BUF_SIZE < bufsize)
		return 0;

	memcpy(stored_data[head], buf, bufsize);
	stored_size[head] = bufsize;

	printf("ringbuff: stored %d bytes at slot %d\n", bufsize, head);
	head++;

	if(RB_NUM_OF_BUFS == head)
		head = 0;

	return bufsize;
}

unsigned int ringbuff_getnext(unsigned char *dest, unsigned int max_destsize)
{
	unsigned int destsize = stored_size[current];

	if( (current == head) || (destsize > max_destsize) )
		return 0;

	memcpy(dest, stored_data[current], destsize);

	printf("ringbuff: got %d bytes from slot %d\n", destsize, current);

	current++;

	if(RB_NUM_OF_BUFS == current)
		current = 0;

	return destsize;
}

