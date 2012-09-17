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

#include "wmbus.h"
#include "wmbus_dvl.h"

// Block1: set manu, id, vers, devtype
static void dvl_prepare_block1(unsigned char *buf, char cfield)
{
	// set C-field
	wmbus_dll_set_c(buf, cfield);
	// set manu
	wmbus_dll_set_manu(buf, "DVL");
	// set id
	wmbus_dll_set_id(buf, 0xDD0001DD); // all dLAN/wM-Bus repeater devices in the AVLN are logically one repeater.
	// set ver: v1
	wmbus_dll_set_version(buf, '1');
	// set device type: OMS repeater type 0x32
	wmbus_dll_set_devtype(buf, 0x32);
}


// create SND-NKE

unsigned int wmbus_dvl_create_snd_nke(unsigned char *sndir, int sndir_len, unsigned char *buf, int buflen)
{
	int len = 22; // excluding len-byte

	dvl_prepare_block1(buf, 0x40 /* SND-NKE */); 
	wmbus_apl_set_ci(buf, 0x80 /* link ext to device 12-byte header */);

	buf[0] = len;
	return len+1;
}

// TODO:
// construct a status message for the devolo repeater
// average send interval maximum: 240 minutes
// devolo manufacturer id: DVL = 0x12CC
// OMS uni-directional repeater device type = 0x32
