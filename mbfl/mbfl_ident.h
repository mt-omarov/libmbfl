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

#ifndef MBFL_IDENT_H
#define MBFL_IDENT_H

/*
 * identify filter
 */

typedef struct _mbfl_identify_filter mbfl_identify_filter;

struct _mbfl_identify_filter {
	void (*filter_ctor)(mbfl_identify_filter *filter);
	void (*filter_dtor)(mbfl_identify_filter *filter);
	int (*filter_function)(int c, mbfl_identify_filter *filter);
	int status;
	int flag;
	int score;
	struct _mbfl_encoding *encoding;
};

typedef struct _mbfl_identify_vtbl mbfl_identify_vtbl;

struct _mbfl_identify_vtbl {
	void (*filter_ctor)(mbfl_identify_filter *filter);
	void (*filter_dtor)(mbfl_identify_filter *filter);
	int (*filter_function)(int c, mbfl_identify_filter *filter);
};

void mbfl_identify_filter_set_vtbl(mbfl_identify_filter *filter, const mbfl_identify_vtbl *vtbl);
void mbfl_identify_filter_select_vtbl(mbfl_identify_filter *filter);
mbfl_identify_filter * mbfl_identify_filter_new(struct _mbfl_encoding *encoding);
void mbfl_identify_filter_delete(mbfl_identify_filter *filter);

void mbfl_filt_ident_common_ctor(mbfl_identify_filter *filter);
void mbfl_filt_ident_common_dtor(mbfl_identify_filter *filter);
void mbfl_filt_ident_false_ctor(mbfl_identify_filter *filter);

int mbfl_filt_ident_false(int c, mbfl_identify_filter *filter);
int mbfl_filt_ident_true(int c, mbfl_identify_filter *filter);

#endif /* MBFL_IDENT_H */