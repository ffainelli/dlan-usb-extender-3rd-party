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
#include "wmbus.h"

void wmbus_hex_dump(unsigned char *buf, int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		printf("0x%02x ", (int) (buf[i]));

		switch(i)
		{
			case 0:
			case 1:
			case 3:
			case 7:
			case 8:
				printf("| "); break;
			case 9:
				printf("|\n"); break;
			
			default:	break;
		}
	}

	printf("\n");
}

/* Wireless M-Bus data link layer */

unsigned char wmbus_dll_get_len(unsigned char *buf)
{
	return buf[0];
}

unsigned char wmbus_dll_get_c(unsigned char *buf)
{
	return buf[1];
}

void wmbus_dll_set_c(unsigned char *buf, char c)
{
	buf[1] = c;
}

unsigned char *wmbus_dll_get_manu(unsigned char *buf)
{
	static unsigned char manu[4];
	unsigned short code = ((unsigned short)buf[3]<<8) | (unsigned short)buf[2]; // byte-order already reversed

	manu[0] = ((code>>10) & 0x1f) | 0x40;
	manu[1] = ((code>> 5) & 0x1f) | 0x40;
	manu[2] = ((code>> 0) & 0x1f) | 0x40;
	manu[3] = 0;

	return manu;
}

void wmbus_dll_set_manu(unsigned char *buf, unsigned char *manu)
{
	unsigned short code;

	code =
		((((unsigned short)(manu[0]))&0x1f)<<10) |
		((((unsigned short)(manu[1]))&0x1f)<<5) |
		((((unsigned short)(manu[2]))&0x1f));

	buf[2] = code & 0xff;
	buf[3] = code >> 8;
}

unsigned long wmbus_dll_get_id(unsigned char *buf)
{
	return (buf[7]<<24) | (buf[6]<<16) | (buf[5]<<8) | (buf[4]<<0);
}

void wmbus_dll_set_id(unsigned char *buf, unsigned long id)
{
	buf[4] = id & 0xFF;
	buf[5] = (id >> 8) & 0xFF;
	buf[6] = (id >> 16) & 0xFF;
	buf[7] = (id >> 24) & 0xFF;
}

unsigned char wmbus_dll_get_version(unsigned char *buf)
{
	return buf[8];
}

void wmbus_dll_set_version(unsigned char *buf, unsigned char ver)
{
	buf[8] = ver;
}

unsigned char wmbus_dll_get_devtype(unsigned char *buf)
{
	return buf[9];
}

void wmbus_dll_set_devtype(unsigned char *buf, unsigned char type)
{
	buf[9] = type;
}

/* Wireless M-Bus Application Layer */

unsigned char wmbus_apl_get_ci(unsigned char *buf)
{
	return buf[10];
}

void wmbus_apl_set_ci(unsigned char *buf, unsigned char ci)
{
	buf[10] = ci;
}

unsigned char wmbus_apl_get_header_length(unsigned char *buf)
{
	unsigned char ci = wmbus_apl_get_ci(buf);

	switch(ci)
	{
		case 0x72: return 12;
		case 0x7a: return 4;
		case 0x80: return 12;
		case 0x8a: return 4;
		case 0x8b: return 12;
		default:   break;
	}
	return 0;
}

/* Wireless M-Bus Application Layer (all headers = 4 and 12 byte headers) */

unsigned char wmbus_apl_get_access_nr(unsigned char *buf)
{
	unsigned char hlen = wmbus_apl_get_header_length(buf);
	if(4>hlen) return 0;
	return buf[11+hlen-4]; // hlen is offset for 12-byte headers
}

void wmbus_apl_set_access_nr(unsigned char *buf, unsigned char acc)
{
	unsigned char hlen = wmbus_apl_get_header_length(buf);
	if(4>hlen) return;
	buf[11+hlen-4] = acc; // hlen is offset for 12-byte headers
}

unsigned char wmbus_apl_get_status(unsigned char *buf)
{
	unsigned char hlen = wmbus_apl_get_header_length(buf);
	if(4>hlen) return 0;
	return buf[12+hlen-4]; // hlen is offset for 12-byte headers
}

void wmbus_apl_set_status(unsigned char *buf, unsigned char status)
{
	unsigned char hlen = wmbus_apl_get_header_length(buf);
	if(4>hlen) return;
	buf[12+hlen-4] = status; // hlen is offset for 12-byte headers
}

unsigned short wmbus_apl_get_signature_word(unsigned char *buf)
{
	unsigned char hlen = wmbus_apl_get_header_length(buf);
	if(4>hlen) return 0;
	unsigned short code = ((unsigned short)buf[14+hlen-4]<<8) | (unsigned short)buf[13+hlen-4];
	return code;
}

unsigned short wmbus_apl_set_signature_word(unsigned char *buf, unsigned short sigword)
{
	unsigned char hlen = wmbus_apl_get_header_length(buf);
	if(4>hlen) return 0;

	buf[13+hlen-4] = (unsigned char) (sigword & 0xf);
	buf[14+hlen-4] = (unsigned char) (sigword >> 8);
	return 1;
}

/* extract details from signature word:
 *  LSB NNNNCCHHb  NNNN=number of enc. blocks, CC=content of telegram, HH=hopcounter
 *  MSB BA00MMMMb  B=bidirectional comm., A=accessibility, 00=reserved, MMMM=enc. mode
 * */

unsigned short wmbus_get_hopcount(unsigned short sigword)
{
	return sigword & 0x03; // bits 0-1
}

unsigned short wmbus_set_hopcount(unsigned short *sigword)
{
	*sigword |= 0x01;
	return *sigword;
}

unsigned short wmbus_get_content_type(unsigned short sigword)
{
	return (sigword>>2) & 0x03; // bits 2-3
}

unsigned short wmbus_get_encblocks(unsigned short sigword)
{
	return (sigword>>4) & 0x0f; // bits 4-7
}

unsigned short wmbus_get_encmode(unsigned short sigword)
{
	return (sigword>>8) & 0x0f; // bits 8-11
}

unsigned short wmbus_get_access(unsigned short sigword)
{
	return (sigword>>14) & 0x01; // bit 14
}

unsigned short wmbus_get_bidir(unsigned short sigword)
{
	return (sigword>>15) & 0x01; // bit 15
}

/* Wireless M-Bus Application Layer (12 byte header only) */

unsigned long wmbus_apl_get_meter_id(unsigned char *buf)
{
	if(12 != wmbus_apl_get_header_length(buf)) return 0;
	return (buf[14]<<24) | (buf[13]<<16) | (buf[12]<<8) | (buf[11]<<0);
}

void wmbus_apl_set_meter_id(unsigned char *buf, unsigned long id)
{
	if(12 != wmbus_apl_get_header_length(buf)) return;
	buf[11] = id & 0xFF;
	buf[12] = (id >> 8) & 0xFF;
	buf[13] = (id >> 16) & 0xFF;
	buf[14] = (id >> 24) & 0xFF;
}

unsigned char *wmbus_apl_get_meter_manu(unsigned char *buf)
{
        if(12 != wmbus_apl_get_header_length(buf)) return 0;

        static unsigned char manu[4];
        unsigned short code = ((unsigned short)buf[16]<<8) | (unsigned short)buf[15]; // byte-order already reversed

        manu[0] = ((code>>10) & 0x1f) | 0x40;
        manu[1] = ((code>> 5) & 0x1f) | 0x40;
        manu[2] = ((code>> 0) & 0x1f) | 0x40;
        manu[3] = 0;

        return manu;
}

void wmbus_apl_set_meter_manu(unsigned char *buf, unsigned char *manu)
{
	if(12 != wmbus_apl_get_header_length(buf)) return;
	unsigned short code;

	code =
		((((unsigned short)(manu[0]))&0x1f)<<10) |
		((((unsigned short)(manu[1]))&0x1f)<<5) |
		((((unsigned short)(manu[2]))&0x1f));

	buf[15] = code & 0xff;
	buf[16] = code >> 8;
}

unsigned char wmbus_apl_get_meter_version(unsigned char *buf)
{
	if(12 != wmbus_apl_get_header_length(buf)) return 0;
	return buf[17];
}

void wmbus_apl_set_meter_version(unsigned char *buf, unsigned char ver)
{
	if(12 != wmbus_apl_get_header_length(buf)) return;
	buf[17] = ver;
}

unsigned char wmbus_apl_get_meter_devtype(unsigned char *buf)
{
	if(12 != wmbus_apl_get_header_length(buf)) return 0;
	return buf[18];
}

void wmbus_apl_set_meter_devtype(unsigned char *buf, unsigned char type)
{
	if(12 != wmbus_apl_get_header_length(buf)) return;
	buf[18] = type;
}

/* OMS utility functions */

/* Only repeat if:
 *  - C field is 0x44 or 0x46
 *  - encryption mode is 0, 5 or 6 (otherwise there is no hop count)
 *  - hop count is 0
 */

int oms_unidir_should_repeat(unsigned char *buf, int buflen)
{
	if(11>buflen) return 0; // length+block1+ci
	
	if(4 > wmbus_apl_get_header_length(buf)) return 0; // 4- or 12-byte header?

	unsigned char C = wmbus_dll_get_c(buf);
	if( (0x44 != C) && (0x46 != C) ) return 0;

	unsigned short sw = wmbus_apl_get_signature_word(buf);

	unsigned short encmode = wmbus_get_encmode(sw);
	if(0!=encmode && 5!=encmode && 6!=encmode) return 0;

	if(0 != wmbus_get_hopcount(sw)) return 0;

	return 1;
}

/* --- */

void wmbus_dump(unsigned char *buf, int buflen)
{
	if(!buflen) return;
	if(11>buflen) return; // length+block1+ci

	unsigned char hlen = wmbus_apl_get_header_length(buf);
	printf("  len=%3d C=0x%02x manu=%s id=0x%08lx ver=0x%02x devtype=0x%02x ci=0x%02x hlen=%02d\n", wmbus_dll_get_len(buf),
		wmbus_dll_get_c(buf), wmbus_dll_get_manu(buf), wmbus_dll_get_id(buf), wmbus_dll_get_version(buf),
		wmbus_dll_get_devtype(buf), wmbus_apl_get_ci(buf), hlen);

	if(12 == hlen)
	{
		printf("  meter -> manu=%s id=0x%08lx ver=0x%02x devtype=0x%02x\n", wmbus_apl_get_meter_manu(buf),
			wmbus_apl_get_meter_id(buf), wmbus_apl_get_meter_version(buf), wmbus_apl_get_meter_devtype(buf));
	}

	if(4 <= hlen)
	{
		unsigned short sigword = wmbus_apl_get_signature_word(buf);
		printf("  AccNr=%03d Status=0x%02x sigword=0x%04x\n  (encmode=%d, encblocks=%d, hopcount=%d, content=0x%x, access=%d, bidir=%d)\n",
			wmbus_apl_get_access_nr(buf),
			wmbus_apl_get_status(buf), sigword, wmbus_get_encmode(sigword), wmbus_get_encblocks(sigword),
			wmbus_get_hopcount(sigword), wmbus_get_content_type(sigword), 
			wmbus_get_access(sigword), wmbus_get_bidir(sigword));
	}

}

