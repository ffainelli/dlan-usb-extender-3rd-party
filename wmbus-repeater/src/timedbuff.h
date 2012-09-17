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
#ifndef _TIMEDBUFF_H
#define _TIMEDBUFF_H

#include <time.h>

int timedbuff_init(void);
unsigned int timedbuff_store(unsigned char *buf, unsigned int bufsize, time_t time_delta);
unsigned int timedbuff_retrieve(unsigned char *data, unsigned int max_datasize);
int timedbuff_update_time(time_t time_elapsed);
time_t timedbuff_get_delay(void);

#endif
