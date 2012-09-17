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

#ifndef _WMBUS_H
#define _WMBUS_H

void wmbus_hex_dump(unsigned char *buf, int len);
unsigned char wmbus_dll_get_len(unsigned char *buf);
unsigned char wmbus_dll_get_c(unsigned char *buf);
void wmbus_dll_set_c(unsigned char *buf, char c);
unsigned char *wmbus_dll_get_manu(unsigned char *buf);
void wmbus_dll_set_manu(unsigned char *buf, unsigned char *manu);
unsigned long wmbus_dll_get_id(unsigned char *buf);
void wmbus_dll_set_id(unsigned char *buf, unsigned long id);
unsigned char wmbus_dll_get_version(unsigned char *buf);
void wmbus_dll_set_version(unsigned char *buf, unsigned char ver);
unsigned char wmbus_dll_get_devtype(unsigned char *buf);
void wmbus_dll_set_devtype(unsigned char *buf, unsigned char type);
unsigned char wmbus_apl_get_ci(unsigned char *buf);
void wmbus_apl_set_ci(unsigned char *buf, unsigned char ci);
unsigned char wmbus_apl_get_header_length(unsigned char *buf);
unsigned char wmbus_apl_get_access_nr(unsigned char *buf);
void wmbus_apl_set_access_nr(unsigned char *buf, unsigned char acc);
unsigned char wmbus_apl_get_status(unsigned char *buf);
void wmbus_apl_set_status(unsigned char *buf, unsigned char status);
unsigned short wmbus_apl_get_signature_word(unsigned char *buf);
unsigned short wmbus_apl_set_signature_word(unsigned char *buf, unsigned short sigword);
unsigned short wmbus_get_hopcount(unsigned short sigword);
unsigned short wmbus_set_hopcount(unsigned short *sigword);
unsigned short wmbus_get_content_type(unsigned short sigword);
unsigned short wmbus_get_encblocks(unsigned short sigword);
unsigned short wmbus_get_encmode(unsigned short sigword);
unsigned short wmbus_get_access(unsigned short sigword);
unsigned short wmbus_get_bidir(unsigned short sigword);
unsigned long wmbus_apl_get_meter_id(unsigned char *buf);
void wmbus_apl_set_meter_id(unsigned char *buf, unsigned long id);
unsigned char *wmbus_apl_get_meter_manu(unsigned char *buf);
void wmbus_apl_set_meter_manu(unsigned char *buf, unsigned char *manu);
unsigned char wmbus_apl_get_meter_version(unsigned char *buf);
void wmbus_apl_set_meter_version(unsigned char *buf, unsigned char ver);
unsigned char wmbus_apl_get_meter_devtype(unsigned char *buf);
void wmbus_apl_set_meter_devtype(unsigned char *buf, unsigned char type);
int oms_unidir_should_repeat(unsigned char *buf, int buflen);
void wmbus_dump(unsigned char *buf, int buflen);

#endif
