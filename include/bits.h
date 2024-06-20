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
// bits.h
// Operations on bitfields
// NOTES:
//   Macros should only ever expand any given argument once if it can be
//   helped so that any arguments with side-effects (like registers that
//   reset themselves) don't cause problems. Of course some already don't.
//
//   The inline functions (defined in bits.c) are intended primarily for
//   situations where type checking the input is desired but also helps to
//   protect against double-expansion of arguments.
//
//   SET_BIT(), CLEAR_BIT(), TOGGLE_BIT(), and MODIFY_BITS() set the variable
//   given to them as the field, nothing else does.
//
//   http:// realtimecollisiondetection.net/blog/?p=78
//   http:// graphics.stanford.edu/~seander/bithacks.html
//   https:// gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
//
//
#ifdef __cplusplus
 extern "C" {
#endif
#ifndef _ULIB_BITS_H
#define _ULIB_BITS_H

#include "src/configify.h"
#if ULIB_ENABLE_BITOPS

#include "types.h"
#include "util.h"

#if ! defined(BITOP_FUNC_INLINE)
# if ULIB_BITOP_ENABLE_INLINED_FUNCTIONS
#  define BITOP_FUNC_INLINE INLINE
# else
#  define BITOP_FUNC_INLINE
# endif
#endif

// Set one or more bits in a field
#ifndef SET_BIT
# define SET_BIT(field, bits) ((field) |= (bits))
#endif
BITOP_FUNC_INLINE uint set_bit(uint field, uint bits);
BITOP_FUNC_INLINE uint8_t set_bit_u8(uint8_t field, uint8_t bits);
BITOP_FUNC_INLINE uint16_t set_bit_u16(uint16_t field, uint16_t bits);
BITOP_FUNC_INLINE uint32_t set_bit_u32(uint32_t field, uint32_t bits);
BITOP_FUNC_INLINE uint64_t set_bit_u64(uint64_t field, uint64_t bits);

// Unset one or more bits in a field
#ifndef CLEAR_BIT
# define CLEAR_BIT(field, bits) ((field) &= ~(bits))
#endif
BITOP_FUNC_INLINE uint clear_bit(uint field, uint bits);
BITOP_FUNC_INLINE uint8_t clear_bit_u8(uint8_t field, uint8_t bits);
BITOP_FUNC_INLINE uint16_t clear_bit_u16(uint16_t field, uint16_t bits);
BITOP_FUNC_INLINE uint32_t clear_bit_u32(uint32_t field, uint32_t bits);
BITOP_FUNC_INLINE uint64_t clear_bit_u64(uint64_t field, uint64_t bits);

// Toggle one or more bits in a field
#define TOGGLE_BIT(field, bits) ((field) ^= (bits))
BITOP_FUNC_INLINE uint toggle_bit(uint field, uint bits);
BITOP_FUNC_INLINE uint8_t toggle_bit_u8(uint8_t field, uint8_t bits);
BITOP_FUNC_INLINE uint16_t toggle_bit_u16(uint16_t field, uint16_t bits);
BITOP_FUNC_INLINE uint32_t toggle_bit_u32(uint32_t field, uint32_t bits);
BITOP_FUNC_INLINE uint64_t toggle_bit_u64(uint64_t field, uint64_t bits);

// Set or clear bits in a field limited by a mask
#define MODIFY_BITS(field, mask, bits) ((field) = (((field) & ~(mask)) | ((bits) & (mask))))
BITOP_FUNC_INLINE uint modify_bits(uint field, uint mask, uint bits);
BITOP_FUNC_INLINE uint8_t modify_bits_u8(uint8_t field, uint8_t mask, uint8_t bits);
BITOP_FUNC_INLINE uint16_t modify_bits_u16(uint16_t field, uint16_t mask, uint16_t bits);
BITOP_FUNC_INLINE uint32_t modify_bits_u32(uint32_t field, uint32_t mask, uint32_t bits);
BITOP_FUNC_INLINE uint64_t modify_bits_u64(uint64_t field, uint64_t mask, uint64_t bits);

// Select every set bit in field that's also set in mask
#define SELECT_BITS(field, mask) ((field) & (mask))
BITOP_FUNC_INLINE uint select_bits(uint field, uint mask);
BITOP_FUNC_INLINE uint8_t select_bits_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE uint16_t select_bits_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE uint32_t select_bits_u32(uint32_t field, uint32_t mask);
BITOP_FUNC_INLINE uint64_t select_bits_u64(uint64_t field, uint64_t mask);

// Select every set bit in field except those set in mask
#define MASK_BITS(field, mask)   ((field) & ~(mask))
BITOP_FUNC_INLINE uint mask_bits(uint field, uint mask);
BITOP_FUNC_INLINE uint8_t mask_bits_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE uint16_t mask_bits_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE uint32_t mask_bits_u32(uint32_t field, uint32_t mask);
BITOP_FUNC_INLINE uint64_t mask_bits_u64(uint64_t field, uint64_t mask);

// Select every bit in a mask at some offset and then shift those bits to the
// LSB end
//#define GATHER_BITS(field, mask, offset) (SELECT_BITS((field), ((mask) << (offset))) >> (offset))
#define GATHER_BITS(field, mask, offset) (((field) >> (offset)) & (mask))
BITOP_FUNC_INLINE uint gather_bits(uint field, uint mask, uint_fast8_t offset);
BITOP_FUNC_INLINE uint8_t gather_bits_u8(uint8_t field, uint8_t mask, uint_fast8_t offset);
BITOP_FUNC_INLINE uint16_t gather_bits_u16(uint16_t field, uint16_t mask, uint_fast8_t offset);
BITOP_FUNC_INLINE uint32_t gather_bits_u32(uint32_t field, uint32_t mask, uint_fast8_t offset);
BITOP_FUNC_INLINE uint64_t gather_bits_u64(uint64_t field, uint64_t mask, uint_fast8_t offset);

// Check if any bit set in mask is set in field
#define BIT_IS_SET(field, mask)    (((field) & (mask)) != 0)
BITOP_FUNC_INLINE bool bit_is_set(uint field, uint mask);
BITOP_FUNC_INLINE bool bit_is_set_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE bool bit_is_set_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE bool bit_is_set_u32(uint32_t field, uint32_t mask);
BITOP_FUNC_INLINE bool bit_is_set_u64(uint64_t field, uint64_t mask);

// Check if every bit set in mask is set in field
// mask is expanded twice for convenience
#define BITS_ARE_SET(field, mask) (((field) & (mask)) == (mask))
BITOP_FUNC_INLINE bool bits_are_set(uint field, uint mask);
BITOP_FUNC_INLINE bool bits_are_set_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE bool bits_are_set_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE bool bits_are_set_u32(uint32_t field, uint32_t mask);
BITOP_FUNC_INLINE bool bits_are_set_u64(uint64_t field, uint64_t mask);

// Turn an integer into a bit mask
#define AS_BIT(n) (1U << (n))
BITOP_FUNC_INLINE uint as_bit(uint_fast8_t n);
BITOP_FUNC_INLINE uint8_t as_bit_u8(uint_fast8_t n);
BITOP_FUNC_INLINE uint16_t as_bit_u16(uint_fast8_t n);
BITOP_FUNC_INLINE uint32_t as_bit_u32(uint_fast8_t n);
BITOP_FUNC_INLINE uint64_t as_bit_u64(uint_fast8_t n);
// The headers for the ATMega provide this. It probably shouldn't be used
// anywhere else since there's an equivalent above, but here it is anyway.
#ifndef _BV
# define _BV(n) (1U << (n))
#endif

// Find the lowest set bit in a field
#define LOWEST_BIT(field) ((field) & -(field))
BITOP_FUNC_INLINE uint lowest_bit(uint field);
BITOP_FUNC_INLINE uint8_t lowest_bit_u8(uint8_t field);
BITOP_FUNC_INLINE uint16_t lowest_bit_u16(uint16_t field);
BITOP_FUNC_INLINE uint32_t lowest_bit_u32(uint32_t field);
BITOP_FUNC_INLINE uint64_t lowest_bit_u64(uint64_t field);
// Same, for unsigned ints, longs, and long longs
#define LSB(x)   (__builtin_ffs(x) - 1)
#define LSBL(x)  (__builtin_ffsl(x) - 1)
#define LSBLL(x) (__builtin_ffsll(x) - 1)

// Shift right so lowest set bit ends up at bit 0
#define SHIFT_LOWEST_BIT(field) ((field) / ((field) & -(field)))
BITOP_FUNC_INLINE uint shift_lowest_bit(uint field);
BITOP_FUNC_INLINE uint8_t shift_lowest_bit_u8(uint8_t field);
BITOP_FUNC_INLINE uint16_t shift_lowest_bit_u16(uint16_t field);
BITOP_FUNC_INLINE uint32_t shift_lowest_bit_u32(uint32_t field);
BITOP_FUNC_INLINE uint64_t shift_lowest_bit_u64(uint64_t field);

// Find the number of leading zeroes in unsigned ints, longs, and long longs
#define CLZ(x)   __builtin_clz(x)
#define CLZL(x)  __builtin_clzl(x)
#define CLZLL(x) __builtin_clzll(x)


#if ULIB_BITOP_ENABLE_INLINED_FUNCTIONS
# include "bits.c"
#endif


#endif // ULIB_ENABLE_BITOPS
#endif // _ULIB__H
#ifdef __cplusplus
 }
#endif
