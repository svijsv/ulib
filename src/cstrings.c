// SPDX-License-Identifier: GPL-3.0-only
/***********************************************************************
*                                                                      *
*                                                                      *
* Copyright 2022, 2023 svijsv                                          *
* This program is free software: you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation, version 3.                             *
*                                                                      *
* This program is distributed in the hope that it will be useful, but  *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
* General Public License for more details.                             *
*                                                                      *
* You should have received a copy of the GNU General Public License    *
* along with this program. If not, see <http:// www.gnu.org/licenses/>.*
*                                                                      *
*                                                                      *
***********************************************************************/
// cstrings.c
// Tools for dealing with C-style strings
// NOTES:
//
//
#ifdef __cplusplus
 extern "C" {
#endif

#include "cstrings.h"
#if ULIB_ENABLE_CSTRINGS

#include "debug.h"

#include <string.h>

static const char CAP_DIFF = 'a' - 'A';

bool cstring_eq(const char *s1, const char *s2) {
	assert(s1 != NULL);
	assert(s2 != NULL);

#if DO_CSTRING_SAFETY_CHECKS
	if ((s1 == NULL) || (s2 == NULL)) {
		return false;
	}
#endif
	return (strcmp(s1, s2) == 0);
}
bool cstring_eqn(const char *s1, const char *s2, uint_t n) {
	assert(s1 != NULL);
	assert(s2 != NULL);

#if DO_CSTRING_SAFETY_CHECKS
	if ((s1 == NULL) || (s2 == NULL)) {
		return false;
	}
#endif
	return (strncmp(s1, s2, n) == 0);
}
bool cstring_eqz(const char *s1, const char *s2) {
	assert(s1 != NULL);
	assert(s2 != NULL);

#if DO_CSTRING_SAFETY_CHECKS
	if ((s1 == NULL) || (s2 == NULL)) {
		return false;
	}
#endif

	const unsigned char *a = (const unsigned char *)s1;
	const unsigned char *b = (const unsigned char *)s2;

	while ((*a != 0) && (*b != 0)) {
		if (*a != *b) {
			return false;
		}
		++a, ++b;
	}
	return (*a == 0);
}
const char* cstring_eat_whitespace(const char *s) {
	assert(s != NULL);

#if DO_CSTRING_SAFETY_CHECKS
	if (s == NULL) {
		return NULL;
	}
#endif

	for (;;) {
		switch (*s) {
			case '\t':
			case '\r':
			case '\n':
			case ' ':
				++s;
				break;
			default:
				return (char *)s;
		}
	}

	return (char *)s;
}
const char* cstring_next_token(const char *cs, char sep) {
	assert(cs != NULL);
	assert(sep != 0);

#if DO_CSTRING_SAFETY_CHECKS
	if (sep == 0) {
		while (*cs != 0) {
			++cs;
		}
		return cs;
	}
	if (cs == NULL) {
		return NULL;
	}
#endif

	// Eat current token
	for (; ((*cs != sep) && (*cs != 0)); ++cs) {
		// Nothing to do in here
	}
	// Eat separators
	for (; (*cs == sep); ++cs) {
		// Nothing to do in here
	}

	return cs;
}
char *cstring_pop_token(char *input, char sep, uint_t *len) {
	uint_t l = 0;
	char *cs = input;

	assert(input != NULL);
	assert(sep != 0);

#if DO_CSTRING_SAFETY_CHECKS
	if (sep == 0) {
		while (*cs != 0) {
			++cs;
		}
		goto END;
	}
	if (input == NULL) {
		if (len != NULL) {
			*len = 0;
		}
		return NULL;
	}
#endif

	// Eat current token
	for (; ((*cs != sep) && (*cs != 0)); ++cs, ++l) {
		// Nothing to do in here
	}
	if (*cs == sep) {
		*cs = 0;
		++cs;
	}

END:
	if (len != NULL) {
		*len = l;
	}
	return cs;
}

const char* cstring_basename(const char *s) {
	const char *bn;
	static const char *dot = ".";

	// basename() accepts null pointers, and we're trying to be like basename().
	/*
	assert(s != NULL);

#if DO_CSTRING_SAFETY_CHECKS
	if (s == NULL) {
		return dot;
	}
#endif
	*/

	if ((s == NULL) || (s[0] == 0)) {
		return dot;
	}

	bn = strrchr(s, '/');
	if (bn == NULL) {
		bn = s;
	} else {
		++bn;
	}
	if (bn[0] == 0) {
		bn = dot;
	}

	return bn;
}
char *cstring_to_upper(char *s) {
	char *c;

	assert(s != NULL);

#if DO_CSTRING_SAFETY_CHECKS
	if (s == NULL) {
		return NULL;
	}
#endif

	for (c = s; *c != 0; ++c) {
		if (*c >= 'a' && *c <= 'z') {
			*c -= CAP_DIFF;
		}
	}

	return s;
}

#else
	// ISO C forbids empty translation units, this makes it happy.
	typedef int make_iso_compilers_happy;
#endif // ULIB_ENABLE_CSTRINGS
#ifdef __cplusplus
 }
#endif
