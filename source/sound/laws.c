/*-----------------------------

SpanDSP - a series of DSP components for telephony
g711.h - In line A-law and u-law conversion routines

Written by Steve Underwood <steveu@coppice.org>

Copyright (C) 2001 Steve Underwood

Despite my general liking of the GPL, I place this code in the
public domain for the benefit of all mankind - even the slimy
ones who might try to proprietize my work and use it to my
detriment.

-------------------------------

 [laws.c]
 - Alexander Brandt 2019

 https://webrtc.googlesource.com/src/+/refs/heads/master/modules/third_party/g711/g711.h
-----------------------------*/

#include "private.h"

#define ULAW_BIAS 0x84
#define ALAW_AMI_MASK 0x55


int16_t AlawToInt16(uint8_t compressed)
{
	compressed ^= ALAW_AMI_MASK;
	int t = ((compressed & 0x0F) << 4);
	int seg = (((int)compressed & 0x70) >> 4);

	if (seg)
		t = (t + 0x108) << (seg - 1);
	else
		t += 8;

	return (int16_t)((compressed & 0x80) ? t : -t);
}


int16_t UlawToInt16(uint8_t compressed)
{
	compressed = ~compressed;
	int t = (((compressed & 0x0F) << 3) + ULAW_BIAS) << (((int)compressed & 0x70) >> 4);

	return (int16_t)((compressed & 0x80) ? (ULAW_BIAS - t) : (t - ULAW_BIAS));
}
