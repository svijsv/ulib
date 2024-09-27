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
// printf.c
// Formatted printing support
//
// NOTES:
//
#ifdef __cplusplus
 extern "C" {
#endif

#include "printf.h"
#if ULIB_ENABLE_PRINTF

#include "debug.h"
#include "ascii.h"
#include "util.h"

#include <limits.h>
#include <string.h>

#define PRINT_BINARY (!PRINTF_USE_MINIMAL_FEATURES)
#define GROUP_1000s  (!PRINTF_USE_MINIMAL_FEATURES)
// This is called 'PAD_STRING' but it controls string max length (precision) too.
// It also controls characters.
#define PAD_STRINGS  (!PRINTF_USE_MINIMAL_FEATURES)
#define ALLOW_LEFT_ADJUST (!PRINTF_USE_MINIMAL_FEATURES)
#define USE_PRECISION (!PRINTF_USE_MINIMAL_FEATURES)
#define TRY_LARGE_INTS (!PRINTF_USE_MINIMAL_FEATURES)

// Not parsing fields when they're ignored causes the arguments to become out
// of sync with va_arg(), which can cause crashes or security problems.
#define PARSE_IGNORED_FIELDS DO_PRINTF_SAFETY_CHECKS


void printf_vv(void (*pputc)(uint_fast8_t c), const char *restrict fmt, ...) {
	va_list arp;

	va_start(arp, fmt);
	printf_va(pputc, fmt, arp);
	va_end(arp);

	return;
}

typedef struct {
	uint_fast8_t width;
	uint_fast8_t precision;
	bool pad_0       :1;
	bool group_1000s :1;
	bool left_adjust :1;
	bool is_signed   :1;
	bool is_negative :1;
	bool lower_hex   :1;
	bool alt_form    :1;
	uint pos_sign    :2;
	uint int_size    :4;
	uint int_base    :5;
} printf_opts_t;

//#define OPTS_DEFAULT { 0, 0, false, false, false, false, false, false, false, 0, 0, 0 }
#define OPTS_DEFAULT { 0 }
/*
static printf_opts_t int_opts_default(void) {
	printf_opts_t def = OPTS_DEFAULT;
	return def;
}
*/

typedef enum {
	POS_SIGN_NONE = 0,
	POS_SIGN_BLANK = 1,
	POS_SIGN_PLUS = 2
} int_sign_t;

#if PRINTF_MAX_INT_BYTES <= 1
 typedef uint_fast8_t printf_uint_t;
 typedef uint_fast8_t printf_int_len_t;
 static const printf_uint_t PRINTF_UINT_MAX = UINT_FAST8_MAX;
 static const printf_uint_t PRINTF_UINT_VALUE_MAX = UINT8_MAX;
#elif PRINTF_MAX_INT_BYTES <= 2
 typedef uint_fast16_t printf_uint_t;
 typedef uint_fast8_t printf_int_len_t;
 static const printf_uint_t PRINTF_UINT_MAX = UINT_FAST16_MAX;
 static const printf_uint_t PRINTF_UINT_VALUE_MAX = UINT16_MAX;
#elif PRINTF_MAX_INT_BYTES <= 4
 typedef uint_fast32_t printf_uint_t;
 typedef uint_fast8_t printf_int_len_t;
 static const printf_uint_t PRINTF_UINT_MAX = UINT_FAST32_MAX;
 static const printf_uint_t PRINTF_UINT_VALUE_MAX = UINT32_MAX;
#elif PRINTF_MAX_INT_BYTES <= 8
 typedef uint_fast64_t printf_uint_t;
 typedef uint_fast16_t printf_int_len_t;
 static const printf_uint_t PRINTF_UINT_MAX = UINT_FAST64_MAX;
 static const printf_uint_t PRINTF_UINT_VALUE_MAX = UINT64_MAX;
#else
# error "Unsupported PRINTF_MAX_INT_BYTES"
#endif

// We can't just just invert a signed int because if it's the maximally negative
// value it can't be represented in positive form without first being copied to
// a larger type. The work-around is to manually convert from two's-complement
// negative to positive after copying to our final (unsigned) variable.
#define GET_SIGNED_INT_ARG(_var_, _type_, _max_neg_) \
	do { \
		_type_ tmp = va_arg(arp, _type_); \
		(_var_) = (printf_uint_t )tmp; \
		if (tmp < 0) { \
			(_var_) ^= PRINTF_UINT_MAX; \
			++(_var_); \
			opts.is_negative = true; \
		} \
	} while (0)
#define GET_UNSIGNED_INT_ARG(_var_, _type_, _va_type_) \
	do { \
		_va_type_ tmp = va_arg(arp, _va_type_); \
		(_var_) = (printf_uint_t )((_type_ )tmp); \
	} while (0)

static void print_int(void(*pputc)(uint_fast8_t c), printf_uint_t n, const printf_opts_t opts) {
	// The largest buffer we need is for the printed binary ints, which require
	// one character per bit. If we're not printing binary though, we use one
	// character per three bits.
#if PRINT_BINARY
	uint8_t print_buf[PRINTF_MAX_INT_BYTES * 8];
#else
	uint8_t print_buf[PRINTF_MAX_INT_BYTES * 3];
#endif
	uint_fast8_t c;
	printf_int_len_t buf_i = 0;
	uint_fast8_t base = opts.int_base;
	uint_fast8_t padding = ' ';

	// Avoid a buffer overflow if someone's converting an unsupported large int
	// while also allowing anything small enough to be printed
	n &= PRINTF_UINT_VALUE_MAX;

	if (!PRINT_BINARY && base == 2) {
		base = 8;
	}

	if (n == 0) {
		print_buf[0] = '0';
		++buf_i;
	} else if (PRINT_BINARY && base == 2) {
		for (; n != 0; ++buf_i, n >>= 1) {
			print_buf[buf_i] = ((n & 0x01U) == 0) ? '0' : '1';
		}
	} else {
		uint_fast8_t xmod = 0;

		if (opts.lower_hex) {
			xmod = 0x20U;
		}
		for (; n != 0; ++buf_i) {
			c = (uint_fast8_t )(n % base);
			n /= base;

			print_buf[buf_i] = ascii_from_xdigit(c) | xmod;
		}
	}

	// We need to know how many characters we are actually going to print in order
	// to pad the output
	// The final buf_i is the index after the last used index, or the total number
	// of characters in the buffer.
	printf_int_len_t print_chars = (printf_int_len_t )MAX(buf_i, opts.precision);

	if (GROUP_1000s && base == 10 && opts.group_1000s && print_chars > 3) {
		print_chars += print_chars / 3;
	}
	if (opts.is_signed && (opts.is_negative || opts.pos_sign != 0)) {
		++print_chars;
	}
	if (opts.alt_form) {
		if ((PRINT_BINARY && base == 2) || base == 16 || (PRINTF_USE_o_FOR_OCTAL && base == 8)) {
			print_chars += 2;
		} else if (base == 8) {
			print_chars += 1;
		}
	}

	if ((!USE_PRECISION || opts.precision == 0) && opts.pad_0) {
		padding = '0';
	}
	bool pad_late = (padding == '0') && ((base != 10 && opts.alt_form) || (base == 10 && opts.is_negative));
	printf_int_len_t pad_chars = (opts.width > print_chars) ? opts.width - print_chars : 0;

	if ((!ALLOW_LEFT_ADJUST || !opts.left_adjust) && !pad_late) {
		for (; pad_chars > 0; --pad_chars) {
			pputc(padding);
		}
	}

	if (opts.is_signed) {
		if (opts.is_negative) {
			pputc('-');
		} else if (opts.pos_sign == POS_SIGN_BLANK) {
			pputc(' ');
		} else if (opts.pos_sign == POS_SIGN_PLUS) {
			pputc('+');
		}
	}

	if (opts.alt_form && base != 10) {
		pputc('0');
		if (PRINT_BINARY && base == 2) {
			pputc('b');
		} else if (PRINTF_USE_o_FOR_OCTAL && base == 8) {
			pputc('o');
		} else if (base == 16) {
			pputc('x');
		}
	}

	if (!ALLOW_LEFT_ADJUST || !opts.left_adjust) {
		for (; pad_chars > 0; --pad_chars) {
			pputc(padding);
		}
	}

	if (USE_PRECISION) {
		for (uint_fast8_t p = opts.precision; p > buf_i; --p) {
			pputc('0');
		}
	}

	// The characters were placed in the buffer from low-digit to high but we want
	// to print them from high-digit to low
	if (GROUP_1000s && base == 10 && opts.group_1000s && buf_i > 3) {
		uint_fast8_t places = (uint_fast8_t )(3 - (buf_i % 3));

		for (; buf_i != 0; --buf_i, ++places) {
			if (places == 3) {
				places = 0;
				if (buf_i > 1) {
					pputc(PRINTF_INT_GROUPING_CHAR);
				}
			}
			pputc(print_buf[buf_i-1]);
		}
	} else {
		for (; buf_i != 0; --buf_i) {
			pputc(print_buf[buf_i-1]);
		}
	}

	if (ALLOW_LEFT_ADJUST) {
		for (; pad_chars > 0; --pad_chars) {
			pputc(' ');
		}
	}

	//return MAX(print_chars, opts.width);
	return;
}

static void print_string(void(*pputc)(uint_fast8_t c), const char *s, const printf_opts_t opts) {
	uint len = 0;

	if (DO_PRINTF_SAFETY_CHECKS && s == NULL) {
		s = "(null)";
	}

	if (PAD_STRINGS) {
		len = (uint )strlen(s);
		if (opts.precision > 0) {
			len = MIN(len, opts.precision);
		}

		uint pad_chars = (opts.width > len) ? opts.width - len : 0;
		if (!ALLOW_LEFT_ADJUST || !opts.left_adjust) {
			for (; pad_chars != 0; --pad_chars) {
				pputc(' ');
			}
		}

		for (uint i = 0; i < len; ++i) {
			pputc((uint_fast8_t )s[i]);
		}

		if (ALLOW_LEFT_ADJUST) {
			for (; pad_chars != 0; --pad_chars) {
				pputc(' ');
			}
		}
	} else {
		//for (; *s != 0; ++s, ++len) {
		for (; *s != 0; ++s) {
			pputc((uint_fast8_t )*s);
		}
	}

	//return MAX(len, opts.width);
	return;
}

static void print_char(void(*pputc)(uint_fast8_t c), char c, const printf_opts_t opts) {
	if (DO_PRINTF_SAFETY_CHECKS && (c == 0)) {
		c = '.';
		//return;
	}

	if (PAD_STRINGS) {
		uint pad_chars = (opts.width > 0) ? opts.width - 1 : 0;

		if (!ALLOW_LEFT_ADJUST || !opts.left_adjust) {
			for (; pad_chars != 0; --pad_chars) {
				pputc(' ');
			}
		}

		pputc((uint_fast8_t )c);

		if (ALLOW_LEFT_ADJUST) {
			for (; pad_chars != 0; --pad_chars) {
				pputc(' ');
			}
		}
	} else {
		pputc((uint_fast8_t )c);
	}

	//return MAX(1, opts.width);
	return;
}

__attribute__((weak))
void printf_va(void(*pputc)(uint_fast8_t c), const char *restrict fmt_s, va_list arp) {
	ulib_assert(pputc != NULL);
	ulib_assert(fmt_s != NULL);

	if (DO_PRINTF_SAFETY_CHECKS) {
		if (pputc == NULL || fmt_s == NULL) {
			//return -EINVAL;
			return;
		}
	}

	const uint8_t *fmt = (uint8_t *)fmt_s;
	while (true) {
		uint_fast8_t c = *fmt++;
		if (c == 0) {
			break;
		}
		if (c != '%') {
			pputc(c);
			continue;
		}
		c = *fmt++;

		//
		// Format specifiers take the form:
		// %[flags][width][.precision][length modifier]conversion
		//
		printf_opts_t opts = OPTS_DEFAULT;

		//
		// Check for flags
		//
		bool done = false;
		while (!done) {
			switch (c) {
			case '0':
				opts.pad_0 = true;
				break;
			case '-':
				opts.left_adjust = true;
				break;
			case ' ':
				opts.pos_sign = POS_SIGN_BLANK;
				break;
			case '+':
				opts.pos_sign = POS_SIGN_PLUS;
				break;
			case '\'':
				opts.group_1000s = true;
				break;
			case '#':
				opts.alt_form = true;
				break;
			default:
				done = true;
				break;
			}
			if (!done) {
				c = *fmt++;
			}
		}

		//
		// Check for width
		//
		if (c >= '0' && c <= '9') {
			uint_fast8_t w = 0;

			while (c >= '0' && c <= '9') {
				w *= 10;
				w += (uint_fast8_t )(c - '0');
				c = *fmt++;
			}
			opts.width = w;
		} else if (c == '*') {
			int w = va_arg(arp, int);

			if (w < 0) {
				w = -w;
				opts.left_adjust = true;
			}
			opts.width = (uint_fast8_t )w;
			c = *fmt++;
		}

		//
		// Check for precision
		// This is the only feature we remove when disabled because it's so much
		// bigger and less common than everything else
#if USE_PRECISION
		if (c == '.') {
			c = *fmt++;
			if (c >= '0' && c <= '9') {
				uint_fast8_t p = 0;

				while (c >= '0' && c <= '9') {
					p *= 10;
					p += (uint_fast8_t )(c - '0');
					c = *fmt++;
				}
				opts.precision = p;
			} else if (c == '*') {
				int p = va_arg(arp, int);

				if (p >= 0) {
					opts.precision = (uint_fast8_t )p;
				}
				c = *fmt++;
			} else {
				opts.precision = 0;
			}
		}
#elif PARSE_IGNORED_FIELDS
		if (c == '.') {
			c = *fmt++;
			if (c == '*') {
				int p = va_arg(arp, int);
				UNUSED(p);
				c = *fmt++;
			} else {
				while (c >= '0' && c <= '9') {
					c = *fmt++;
				}
			}
		}
#endif

		//
		// Check for length modifier
		//
		switch (c) {
		case 'h':
			if (*fmt == 'h') {
				opts.int_size = sizeof(char);
				fmt++;
			} else {
				opts.int_size = sizeof(short);
			}
			break;
		case 'l':
			if (*fmt == 'l') {
				opts.int_size = sizeof(long long);
				fmt++;
			} else {
				opts.int_size = sizeof(long);
			}
			break;
		case 'j':
			opts.int_size = sizeof(intmax_t);
			break;
		case 'z':
			opts.int_size = sizeof(size_t);
			break;
		case 't':
			opts.int_size = sizeof(ptrdiff_t);
			break;
		/*
		case 'L':
			opts.int_size = sizeof(long double);
			break;
		*/
		}
		if (opts.int_size == 0) {
			opts.int_size = sizeof(int);
		} else {
			c = *fmt++;
		}

		//
		// Finally, check for the conversion specifier
		//
		switch (c) {
		case 0:
			break;

		case 'd':
		case 'i':
			opts.is_signed = true;
			opts.int_base = 10;
			break;
		case 'b':
			opts.int_base = 2;
			break;
		case 'o':
			opts.int_base = 8;
			break;
		case 'x':
			opts.lower_hex = true;
			opts.int_base = 16;
			break;
		case 'X':
			opts.int_base = 16;
			break;
		case 'u':
			opts.int_base = 10;
			break;

		case 'c':
			//pputc(va_arg(arp, unsigned char));
			print_char(pputc, (char )va_arg(arp, int), opts);
			break;

		case 's':
			print_string(pputc, va_arg(arp, const char *), opts);
			break;

		case '%':
			pputc('%');
			break;

#if PARSE_IGNORED_FIELDS
		case 'n':
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
		case 'a':
		case 'A':
		case 'p':
		case 'm':
		{
			int tmp = va_arg(arp, int);
			UNUSED(tmp);
			break;
		}
#endif

		default:
			pputc('%');
			pputc(c);
			break;
		}
		if (c == 0) {
			break;
		}

		if (opts.int_base > 0) {
			printf_uint_t n = 0;

			// The integer arguments to a variadic function are promoted if smaller
			// than int so we need to read them as ints.
			if (opts.is_signed) {
				switch (opts.int_size) {
				case 1:
					//GET_SIGNED_INT_ARG(n, int8_t);
					GET_SIGNED_INT_ARG(n, int, UINT_MAX);
					break;
#if TRY_LARGE_INTS || PRINTF_MAX_INT_BYTES >= 2
				case 2:
					//GET_SIGNED_INT_ARG(n, int16_t);
					GET_SIGNED_INT_ARG(n, int, UINT_MAX);
					break;
#endif
#if TRY_LARGE_INTS || PRINTF_MAX_INT_BYTES >= 4
				case 4:
					GET_SIGNED_INT_ARG(n, int32_t, UINT32_MAX);
					break;
#endif
#if TRY_LARGE_INTS || PRINTF_MAX_INT_BYTES >= 8
				case 8:
					GET_SIGNED_INT_ARG(n, int64_t, UINT64_MAX);
					break;
#endif
				}
			} else {
				switch (opts.int_size) {
				case 1:
					//GET_UNSIGNED_INT_ARG(n, uint8_t);
					GET_UNSIGNED_INT_ARG(n, uint8_t, int);
					break;
#if TRY_LARGE_INTS || PRINTF_MAX_INT_BYTES >= 2
				case 2:
					//GET_UNSIGNED_INT_ARG(n, uint16_t);
					GET_UNSIGNED_INT_ARG(n, uint16_t, int);
					break;
#endif
#if TRY_LARGE_INTS || PRINTF_MAX_INT_BYTES >= 4
				case 4:
					GET_UNSIGNED_INT_ARG(n, uint32_t, uint32_t);
					break;
#endif
#if TRY_LARGE_INTS || PRINTF_MAX_INT_BYTES >= 8
				case 8:
					GET_UNSIGNED_INT_ARG(n, uint64_t, uint64_t);
					break;
#endif
				}
			}

			//char_count += print_int(pputc, n, opts);
			print_int(pputc, n, opts);
		}
	}

	return;
}

#else
	// ISO C forbids empty translation units, this makes it happy.
	typedef int make_iso_compilers_happy;
#endif // ULIB_ENABLE_PRINTF
#ifdef __cplusplus
 }
#endif
