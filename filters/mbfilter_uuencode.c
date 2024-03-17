/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this file:
 *
 */
/*
 * The source code included in this files was separated from mbfilter.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_uuencode.h"

static size_t mb_uuencode_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);

const mbfl_encoding mbfl_encoding_uuencode = {
	mbfl_no_encoding_uuencode,
	"UUENCODE",
	"x-uuencode",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS,
	NULL,
	NULL,
	mb_uuencode_to_wchar,
	NULL,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_uuencode_8bit = {
	mbfl_no_encoding_uuencode,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_uudec,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

#define UUDEC(c)	(char)(((c)-' ') & 077)
static const char *uuenc_begin_text = "begin ";
enum {
	uudec_state_ground=0,
	uudec_state_inbegin,
	uudec_state_until_newline,
	uudec_state_size,
	uudec_state_a,
	uudec_state_b,
	uudec_state_c,
	uudec_state_d,
	uudec_state_skip_newline
};

int mbfl_filt_conv_uudec(int c, mbfl_convert_filter *filter)
{
	int n;

	switch(filter->status)	{
		case uudec_state_ground:
			/* looking for "begin 0666 filename\n" line */
			if (filter->cache == 0 && c == 'b')
			{
				filter->status = uudec_state_inbegin;
				filter->cache = 1; /* move to 'e' */
			}
			else if (c == '\n')
				filter->cache = 0;
			else
				filter->cache++;
			break;
		case uudec_state_inbegin:
			if (uuenc_begin_text[filter->cache++] != c)	{
				/* doesn't match pattern */
				filter->status = uudec_state_ground;
				break;
			}
			if (filter->cache == 5)
			{
				/* that's good enough - wait for a newline */
				filter->status = uudec_state_until_newline;
				filter->cache = 0;
			}
			break;
		case uudec_state_until_newline:
			if (c == '\n')
				filter->status = uudec_state_size;
			break;
		case uudec_state_size:
			/* get "size" byte */
			n = UUDEC(c);
			filter->cache = n << 24;
			filter->status = uudec_state_a;
			break;
		case uudec_state_a:
			/* get "a" byte */
			n = UUDEC(c);
			filter->cache |= (n << 16);
			filter->status = uudec_state_b;
			break;
		case uudec_state_b:
			/* get "b" byte */
			n = UUDEC(c);
			filter->cache |= (n << 8);
			filter->status = uudec_state_c;
			break;
		case uudec_state_c:
			/* get "c" byte */
			n = UUDEC(c);
			filter->cache |= n;
			filter->status = uudec_state_d;
			break;
		case uudec_state_d:
			/* get "d" byte */
			{
				int A, B, C, D = UUDEC(c);
				A = (filter->cache >> 16) & 0xff;
				B = (filter->cache >> 8) & 0xff;
				C = (filter->cache) & 0xff;
				n = (filter->cache >> 24) & 0xff;
				if (n-- > 0)
					CK((*filter->output_function)( (A << 2) | (B >> 4), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (B << 4) | (C >> 2), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (C << 6) | D, filter->data));
				if (n < 0)
					n = 0;
				filter->cache = n << 24;

				if (n == 0)
					filter->status = uudec_state_skip_newline;	/* skip next byte (newline) */
				else
					filter->status = uudec_state_a; /* go back to fetch "A" byte */
			}
			break;
		case uudec_state_skip_newline:
			/* skip newline */
			filter->status = uudec_state_size;
	}
	return 0;
}

/* Using mbstring to decode UUEncoded text is already deprecated
 * However, to facilitate the move to the new, faster internal conversion interface,
 * We will temporarily implement it for UUEncode */

static size_t mb_uuencode_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	assert(bufsize >= 3);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	unsigned int _state = *state & 0xFF;
	unsigned int size = *state >> 8;

	while (p < e && (limit - out) >= 3) {
		unsigned char c = *p++;

		switch (_state) {
		case uudec_state_ground:
			if (c == 'b') {
				if ((e - p) >= 5 && memcmp(p, uuenc_begin_text+1, 5) == 0) {
					p += 5;
					while (p < e && *p++ != '\n'); /* Consume everything up to newline */
					_state = uudec_state_size;
				}
				/* We didn't find "begin " */
			}
			break;

		case uudec_state_size:
			size = UUDEC(c);
			_state = uudec_state_a;
			break;

		case uudec_state_a:
			if ((e - p) < 4) {
				p = e;
				break;
			}

			unsigned int a = UUDEC(c);
			unsigned int b = UUDEC(*p++);
			unsigned int c = UUDEC(*p++);
			unsigned int d = UUDEC(*p++);

			if (size > 0) {
				*out++ = ((a << 2) | (b >> 4)) & 0xFF;
				size--;
			}
			if (size > 0) {
				*out++ = ((b << 4) | (c >> 2)) & 0xFF;
				size--;
			}
			if (size > 0) {
				*out++ = ((c << 6) | d) & 0xFF;
				size--;
			}

			_state = size ? uudec_state_a : uudec_state_skip_newline;
			break;

		case uudec_state_skip_newline:
			_state = uudec_state_size;
			break;
		}
	}

	*state = (size << 8) | _state;
	*in_len = e - p;
	*in = p;
	return out - buf;
}

static unsigned char uuencode_six_bits(unsigned int bits)
{
	if (bits == 0) {
		return '`';
	} else {
		return bits + 32;
	}
}
