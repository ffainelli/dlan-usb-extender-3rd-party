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
#ifndef _AMBER_H
#define _AMBER_H

typedef enum 
{
	mode_unknown = 0x00,
	mode_S1 = 0x01,
	mode_S2 = 0x03,
	mode_T1meter = 0x05,
	mode_T1other = 0x06,
	mode_T2meter = 0x07,
	mode_T2other = 0x08,
	mode_retain = 0xff
} tWMBUS_MODE;

int amber_open(const char *devname, const char *mode);
int amber_close(void);
int amber_write(unsigned char *buf, int buflen);
int amber_write_command(unsigned char *buf, int buflen);
int amber_read(unsigned char *buf, int maxbuflen);
int amber_get_fd(void);
int amber_fd_is_valid(const char *device);

#endif

