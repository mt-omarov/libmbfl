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
 * The source code included in this files was separated from mbfilter.h
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.h is included in this package .
 *
 */

#ifndef MBFL_ENCODING_H
#define MBFL_ENCODING_H

#include "mbfl_defs.h"
#include "mbfl_consts.h"

enum mbfl_no_encoding {
	mbfl_no_encoding_invalid = -1,
	mbfl_no_encoding_pass,
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_uuencode,
	mbfl_no_encoding_html_ent,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_7bit,
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_charset_min,
	mbfl_no_encoding_ucs4,
	mbfl_no_encoding_ucs4be,
	mbfl_no_encoding_ucs4le,
	mbfl_no_encoding_ucs2,
	mbfl_no_encoding_ucs2be,
	mbfl_no_encoding_ucs2le,
	mbfl_no_encoding_utf32,
	mbfl_no_encoding_utf32be,
	mbfl_no_encoding_utf32le,
	mbfl_no_encoding_utf16,
	mbfl_no_encoding_utf16be,
	mbfl_no_encoding_utf16le,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_utf8_docomo,
	mbfl_no_encoding_utf8_kddi_a,
	mbfl_no_encoding_utf8_kddi_b,
	mbfl_no_encoding_utf8_sb,
	mbfl_no_encoding_utf7,
	mbfl_no_encoding_utf7imap,
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_eucjp2004,
	mbfl_no_encoding_sjis,
	mbfl_no_encoding_eucjp_win,
 	mbfl_no_encoding_sjis_docomo,
 	mbfl_no_encoding_sjis_kddi,
 	mbfl_no_encoding_sjis_sb,
 	mbfl_no_encoding_sjis_mac,
	mbfl_no_encoding_sjis2004,
	mbfl_no_encoding_cp932,
	mbfl_no_encoding_sjiswin,
	mbfl_no_encoding_cp51932,
	mbfl_no_encoding_jis,
	mbfl_no_encoding_2022jp,
	mbfl_no_encoding_2022jp_2004,
	mbfl_no_encoding_2022jp_kddi,
	mbfl_no_encoding_2022jpms,
	mbfl_no_encoding_gb18030,
	mbfl_no_encoding_cp1252,
	mbfl_no_encoding_cp1254,
	mbfl_no_encoding_8859_1,
	mbfl_no_encoding_8859_2,
	mbfl_no_encoding_8859_3,
	mbfl_no_encoding_8859_4,
	mbfl_no_encoding_8859_5,
	mbfl_no_encoding_8859_6,
	mbfl_no_encoding_8859_7,
	mbfl_no_encoding_8859_8,
	mbfl_no_encoding_8859_9,
	mbfl_no_encoding_8859_10,
	mbfl_no_encoding_8859_13,
	mbfl_no_encoding_8859_14,
	mbfl_no_encoding_8859_15,
	mbfl_no_encoding_euc_cn,
	mbfl_no_encoding_cp936,
	mbfl_no_encoding_euc_tw,
	mbfl_no_encoding_big5,
	mbfl_no_encoding_cp950,
	mbfl_no_encoding_euc_kr,
	mbfl_no_encoding_2022kr,
	mbfl_no_encoding_uhc,
	mbfl_no_encoding_hz,
	mbfl_no_encoding_cp1251,
	mbfl_no_encoding_cp866,
	mbfl_no_encoding_koi8r,
	mbfl_no_encoding_koi8u,
	mbfl_no_encoding_8859_16,
	mbfl_no_encoding_armscii8,
	mbfl_no_encoding_cp850,
	mbfl_no_encoding_cp50220,
	mbfl_no_encoding_cp50221,
	mbfl_no_encoding_cp50222,
	mbfl_no_encoding_charset_max
};

struct _mbfl_convert_filter;
struct mbfl_convert_vtbl {
	enum mbfl_no_encoding from;
	enum mbfl_no_encoding to;
	void (*filter_ctor)(struct _mbfl_convert_filter *filter);
	void (*filter_dtor)(struct _mbfl_convert_filter *filter);
	int (*filter_function)(int c, struct _mbfl_convert_filter *filter);
	int (*filter_flush)(struct _mbfl_convert_filter *filter);
	void (*filter_copy)(struct _mbfl_convert_filter *src, struct _mbfl_convert_filter *dest);
};

typedef struct {
	unsigned char *out;
	unsigned char *limit;
	uint32_t state;
	uint32_t errors;
	uint32_t replacement_char;
	unsigned int error_mode;
	unsigned char *str;
} mb_convert_buf;

typedef size_t (*mb_to_wchar_fn)(unsigned char **in, size_t *in_len, uint32_t *out, size_t out_len, unsigned int *state);
typedef bool (*mb_check_fn)(unsigned char *in, size_t in_len);

/* When converting encoded text to a buffer of wchars (Unicode codepoints) using `mb_to_wchar_fn`,
 * the buffer must be at least this size (to work with all supported text encodings) */
#define MBSTRING_MIN_WCHAR_BUFSIZE 5

static inline void mb_convert_buf_init(mb_convert_buf *buf, size_t initsize, uint32_t repl_char, unsigned int err_mode)
{
	buf->state = buf->errors = 0;
	buf->str = (mbfl_convert_filter *)mbfl_realloc(initsize);
	buf->out = buf->str;
	buf->limit = buf->out + initsize;
	buf->replacement_char = repl_char;
	buf->error_mode = err_mode;
}

static inline unsigned char* mb_convert_buf_add(unsigned char *out, char c)
{
	*out++ = c;
	return out;
}

static inline unsigned char* mb_convert_buf_add2(unsigned char *out, char c1, char c2)
{
	*out++ = c1;
	*out++ = c2;
	return out;
}

static inline unsigned char* mb_convert_buf_add3(unsigned char *out, char c1, char c2, char c3)
{
	*out++ = c1;
	*out++ = c2;
	*out++ = c3;
	return out;
}

static inline unsigned char* mb_convert_buf_add4(unsigned char *out, char c1, char c2, char c3, char c4)
{
	*out++ = c1;
	*out++ = c2;
	*out++ = c3;
	*out++ = c4;
	return out;
}

static inline unsigned char* mb_convert_buf_appends(unsigned char *out, const char *s)
{
	while (*s) {
		*out++ = *s++;
	}
	return out;
}

static inline unsigned char* mb_convert_buf_appendn(unsigned char *out, const char *s, size_t n)
{
	while (n--) {
		*out++ = *s++;
	}
	return out;
}

typedef struct {
	enum mbfl_no_encoding no_encoding;
	const char *name;
	const char *mime_name;
	const char **aliases;
	const unsigned char *mblen_table;
	unsigned int flag;
	const struct mbfl_convert_vtbl *input_filter;
	const struct mbfl_convert_vtbl *output_filter;
	mb_to_wchar_fn to_wchar;
	mb_check_fn check;
} mbfl_encoding;

extern const mbfl_encoding mbfl_encoding_utf8;

static inline void mb_convert_buf_free(mb_convert_buf *buf)
{
	mbfl_free((void*)buf->str);
}

static inline size_t mb_convert_buf_len(mb_convert_buf *buf)
{
	return buf->out - buf->str;
}

static inline void mb_convert_buf_reset(mb_convert_buf *buf, size_t len)
{
	buf->out = buf->str + len;
	assert(buf->out <= buf->limit);
}

MBFLAPI extern const mbfl_encoding *mbfl_name2encoding(const char *name);
MBFLAPI extern const mbfl_encoding *mbfl_no2encoding(enum mbfl_no_encoding no_encoding);
MBFLAPI extern const mbfl_encoding **mbfl_get_supported_encodings(void);
MBFLAPI extern const char *mbfl_no_encoding2name(enum mbfl_no_encoding no_encoding);
MBFLAPI extern const char *mbfl_encoding_preferred_mime_name(const mbfl_encoding *encoding);

#endif /* MBFL_ENCODING_H */
