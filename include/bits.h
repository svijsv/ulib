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
#define _SET_BIT(field, bits) ((field) |= (bits))
BITOP_FUNC_INLINE uint set_bit_ui(uint field, uint bits);
BITOP_FUNC_INLINE uint8_t set_bit_u8(uint8_t field, uint8_t bits);
BITOP_FUNC_INLINE uint16_t set_bit_u16(uint16_t field, uint16_t bits);
BITOP_FUNC_INLINE uint32_t set_bit_u32(uint32_t field, uint32_t bits);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t set_bit_u64(uint64_t field, uint64_t bits);
#endif

// Unset one or more bits in a field
#define _CLEAR_BIT(field, bits) ((field) &= ~(bits))
BITOP_FUNC_INLINE uint clear_bit_ui(uint field, uint bits);
BITOP_FUNC_INLINE uint8_t clear_bit_u8(uint8_t field, uint8_t bits);
BITOP_FUNC_INLINE uint16_t clear_bit_u16(uint16_t field, uint16_t bits);
BITOP_FUNC_INLINE uint32_t clear_bit_u32(uint32_t field, uint32_t bits);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t clear_bit_u64(uint64_t field, uint64_t bits);
#endif

// Toggle one or more bits in a field
#define _TOGGLE_BIT(field, bits) ((field) ^= (bits))
BITOP_FUNC_INLINE uint toggle_bit_ui(uint field, uint bits);
BITOP_FUNC_INLINE uint8_t toggle_bit_u8(uint8_t field, uint8_t bits);
BITOP_FUNC_INLINE uint16_t toggle_bit_u16(uint16_t field, uint16_t bits);
BITOP_FUNC_INLINE uint32_t toggle_bit_u32(uint32_t field, uint32_t bits);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t toggle_bit_u64(uint64_t field, uint64_t bits);
#endif

// Set or clear bits in a field limited by a mask
#define _MODIFY_BITS(field, mask, bits) ((field) = (((field) & ~(mask)) | ((bits) & (mask))))
BITOP_FUNC_INLINE uint modify_bits_ui(uint field, uint mask, uint bits);
BITOP_FUNC_INLINE uint8_t modify_bits_u8(uint8_t field, uint8_t mask, uint8_t bits);
BITOP_FUNC_INLINE uint16_t modify_bits_u16(uint16_t field, uint16_t mask, uint16_t bits);
BITOP_FUNC_INLINE uint32_t modify_bits_u32(uint32_t field, uint32_t mask, uint32_t bits);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t modify_bits_u64(uint64_t field, uint64_t mask, uint64_t bits);
#endif

// Select every set bit in field that's also set in mask
#define SELECT_BITS(field, mask) ((field) & (mask))
BITOP_FUNC_INLINE uint select_bits_ui(uint field, uint mask);
BITOP_FUNC_INLINE uint8_t select_bits_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE uint16_t select_bits_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE uint32_t select_bits_u32(uint32_t field, uint32_t mask);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t select_bits_u64(uint64_t field, uint64_t mask);
#endif

// Select every set bit in field except those set in mask
#define MASK_BITS(field, mask)   ((field) & ~(mask))
BITOP_FUNC_INLINE uint mask_bits_ui(uint field, uint mask);
BITOP_FUNC_INLINE uint8_t mask_bits_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE uint16_t mask_bits_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE uint32_t mask_bits_u32(uint32_t field, uint32_t mask);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t mask_bits_u64(uint64_t field, uint64_t mask);
#endif

// Select every bit in a mask at some offset and then shift those bits to the
// LSB end
//#define GATHER_BITS(field, mask, offset) (SELECT_BITS((field), ((mask) << (offset))) >> (offset))
#define GATHER_BITS(field, mask, offset) (((field) >> (offset)) & (mask))
BITOP_FUNC_INLINE uint gather_bits_ui(uint field, uint mask, uint_fast8_t offset);
BITOP_FUNC_INLINE uint8_t gather_bits_u8(uint8_t field, uint8_t mask, uint_fast8_t offset);
BITOP_FUNC_INLINE uint16_t gather_bits_u16(uint16_t field, uint16_t mask, uint_fast8_t offset);
BITOP_FUNC_INLINE uint32_t gather_bits_u32(uint32_t field, uint32_t mask, uint_fast8_t offset);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t gather_bits_u64(uint64_t field, uint64_t mask, uint_fast8_t offset);
#endif

// Check if any bit set in mask is set in field
#define BIT_IS_SET(field, mask)    (((field) & (mask)) != 0)
BITOP_FUNC_INLINE bool bit_is_set_ui(uint field, uint mask);
BITOP_FUNC_INLINE bool bit_is_set_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE bool bit_is_set_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE bool bit_is_set_u32(uint32_t field, uint32_t mask);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE bool bit_is_set_u64(uint64_t field, uint64_t mask);
#endif

// Check if every bit set in mask is set in field
// mask is expanded twice for convenience
#define BITS_ARE_SET(field, mask) (((field) & (mask)) == (mask))
BITOP_FUNC_INLINE bool bits_are_set_ui(uint field, uint mask);
BITOP_FUNC_INLINE bool bits_are_set_u8(uint8_t field, uint8_t mask);
BITOP_FUNC_INLINE bool bits_are_set_u16(uint16_t field, uint16_t mask);
BITOP_FUNC_INLINE bool bits_are_set_u32(uint32_t field, uint32_t mask);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE bool bits_are_set_u64(uint64_t field, uint64_t mask);
#endif

// Turn an integer into a bit mask
#define AS_BIT(n) (1U << (n))
BITOP_FUNC_INLINE uint as_bit_ui(uint_fast8_t n);
BITOP_FUNC_INLINE uint8_t as_bit_u8(uint_fast8_t n);
BITOP_FUNC_INLINE uint16_t as_bit_u16(uint_fast8_t n);
BITOP_FUNC_INLINE uint32_t as_bit_u32(uint_fast8_t n);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t as_bit_u64(uint_fast8_t n);
#endif
/*
// The headers for the ATMega provide this. It probably shouldn't be used
// anywhere else since there's an equivalent above, but here it is anyway.
#ifndef _BV
# define _BV(n) (1U << (n))
#endif
*/

// Find the lowest set bit in a field
#define LOWEST_BIT(field) ((field) & -(field))
BITOP_FUNC_INLINE uint lowest_bit_ui(uint field);
BITOP_FUNC_INLINE uint8_t lowest_bit_u8(uint8_t field);
BITOP_FUNC_INLINE uint16_t lowest_bit_u16(uint16_t field);
BITOP_FUNC_INLINE uint32_t lowest_bit_u32(uint32_t field);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t lowest_bit_u64(uint64_t field);
#endif
// Same, for unsigned ints, longs, and long longs
#define LSB(x)   (__builtin_ffs(x) - 1)
#define LSBL(x)  (__builtin_ffsl(x) - 1)
#define LSBLL(x) (__builtin_ffsll(x) - 1)

// Shift right so lowest set bit ends up at bit 0
#define SHIFT_LOWEST_BIT(field) ((field) / ((field) & -(field)))
BITOP_FUNC_INLINE uint shift_lowest_bit_ui(uint field);
BITOP_FUNC_INLINE uint8_t shift_lowest_bit_u8(uint8_t field);
BITOP_FUNC_INLINE uint16_t shift_lowest_bit_u16(uint16_t field);
BITOP_FUNC_INLINE uint32_t shift_lowest_bit_u32(uint32_t field);
#if ULIB_BITOP_ENABLE_INLINED_64BIT_FUNCTIONS
BITOP_FUNC_INLINE uint64_t shift_lowest_bit_u64(uint64_t field);
#endif

// Find the number of leading zeroes in unsigned ints, longs, and long longs
#define CLZ(x)   __builtin_clz(x)
#define CLZL(x)  __builtin_clzl(x)
#define CLZLL(x) __builtin_clzll(x)

//
// Generic function callers
// I would prefer if there were no default, but some platforms consider uint
// to be a separate type from the size-specified equivalent and others don't
// and there doesn't seem to be a better workaround.
//
#if ULIB_BITOP_ENABLE_GENERICS
# define set_bit(_f_, _b_) _Generic((_f_), \
		uint8_t : set_bit_u8,  \
		uint16_t: set_bit_u16, \
		uint32_t: set_bit_u32, \
		uint64_t: set_bit_u64, \
		default : set_bit_ui  \
	)((_f_), (_b_))
# define SET_BIT(_f_, _b_) ((_f_) = (set_bit((_f_), (_b_))))

# define clear_bit(_f_, _b_) _Generic((_f_), \
		uint8_t : clear_bit_u8,  \
		uint16_t: clear_bit_u16, \
		uint32_t: clear_bit_u32, \
		uint64_t: clear_bit_u64, \
		default : clear_bit_ui  \
	)((_f_), (_b_))
# define CLEAR_BIT(_f_, _b_) ((_f_) = (clear_bit((_f_), (_b_))))

# define toggle_bit(_f_, _b_) _Generic((_f_), \
		uint8_t : toggle_bit_u8,  \
		uint16_t: toggle_bit_u16, \
		uint32_t: toggle_bit_u32, \
		uint64_t: toggle_bit_u64, \
		default : toggle_bit_ui  \
	)((_f_), (_b_))
# define TOGGLE_BIT(_f_, _b_) ((_f_) = (toggle_bit((_f_), (_b_))))

# define modify_bits(_f_, _m_, _b_) _Generic((_f_), \
		uint8_t : modify_bits_u8,  \
		uint16_t: modify_bits_u16, \
		uint32_t: modify_bits_u32, \
		uint64_t: modify_bits_u64, \
		default : modify_bits_ui  \
	)((_f_), (_m_), (_b_))
# define MODIFY_BITS(_f_, _m_, _b_) ((_f_) = (modify_bits((_f_), (_m_), (_b_))))

# define select_bits(_f_, _m_) _Generic((_f_), \
		uint8_t : select_bits_u8,  \
		uint16_t: select_bits_u16, \
		uint32_t: select_bits_u32, \
		uint64_t: select_bits_u64, \
		default : select_bits_ui  \
	)((_f_), (_m_))

# define mask_bits(_f_, _m_) _Generic((_f_), \
		uint8_t : mask_bits_u8,  \
		uint16_t: mask_bits_u16, \
		uint32_t: mask_bits_u32, \
		uint64_t: mask_bits_u64, \
		default : mask_bits_ui  \
	)((_f_), (_m_))

# define gather_bits(_f_, _m_, _o_) _Generic((_f_), \
		uint8_t : gather_bits_u8,  \
		uint16_t: gather_bits_u16, \
		uint32_t: gather_bits_u32, \
		uint64_t: gather_bits_u64, \
		default : gather_bits_ui  \
	)((_f_), (_m_), (_o_))

# define bit_is_set(_f_, _m_) _Generic((_f_), \
		uint8_t : bit_is_set_u8,  \
		uint16_t: bit_is_set_u16, \
		uint32_t: bit_is_set_u32, \
		uint64_t: bit_is_set_u64, \
		default : bit_is_set_ui  \
	)((_f_), (_m_))

# define bits_are_set(_f_, _m_) _Generic((_f_), \
		uint8_t : bits_are_set_u8,  \
		uint16_t: bits_are_set_u16, \
		uint32_t: bits_are_set_u32, \
		uint64_t: bits_are_set_u64, \
		default : bits_are_set_ui  \
	)((_f_), (_m_))

# define as_bit(_n_) _Generic((_n_), \
		uint8_t : as_bit_u8,  \
		uint16_t: as_bit_u16, \
		uint32_t: as_bit_u32, \
		uint64_t: as_bit_u64, \
		default : as_bit_ui  \
	)((_n_))

# define lowest_bit(_f_) _Generic((_f_), \
		uint8_t : LSB,  \
		uint16_t: LSB, \
		uint32_t: LSBL, \
		uint64_t: LSBLL, \
		default : LSB  \
	)((_f_))

# define shift_lowest_bit(_f_) _Generic((_f_), \
		uint8_t : shift_lowest_bit_u8,  \
		uint16_t: shift_lowest_bit_u16, \
		uint32_t: shift_lowest_bit_u32, \
		uint64_t: shift_lowest_bit_u64, \
		default : shift_lowest_bit_ui,  \
	)((_f_))

# define clz(_f_) _Generic((_f_), \
		uint8_t : CLZ,  \
		uint16_t: CLZ, \
		uint32_t: CLZL, \
		uint64_t: CLZLL, \
		default : CLZ  \
	)((_f_))

#else // ULIB_BITOP_ENABLE_GENERICS
# define set_bit(_f_, _b_) set_bit_ui((_f_), (_b_))
# define SET_BIT(_f_, _b_) (_SET_BIT((_f_), (_b_)))

# define clear_bit(_f_, _b_) clear_bit_ui((_f_), (_b_))
# define CLEAR_BIT(_f_, _b_) (_CLEAR_BIT((_f_), (_b_)))

# define toggle_bit(_f_, _b_) toggle_bit_ui((_f_), (_b_))
# define TOGGLE_BIT(_f_, _b_) (_TOGGLE_BIT((_f_), (_b_)))

# define modify_bits(_f_, _m_, _b_) modify_bits_ui((_f_), (_m_), (_b_))
# define MODIFY_BITS(_f_, _m_, _b_) (_MODIFY_BITS((_f_), (_m_), (_b_)))

# define select_bits(_f_, _m_) select_bit_ui((_f_), (_m_))
# define mask_bits(_f_, _m_) mask_bits_ui((_f_), (_m_))
# define gather_bits(_f_, _m_, _o_) gather_bits_ui((_f_), (_m_), (_o_))
# define bit_is_set(_f_, _m_) bit_is_set_ui((_f_), (_m_))
# define bits_are_set(_f_, _m_) bits_are_set_ui((_f_), (_m_))
# define as_bit(_n_) as_bit_ui((_n_))
# define lowest_bit(_f_) lowest_bit_ui((_f_))
# define shift_lowest_bit(_f_) shift_lowest_bit_ui((_f_))
# define clz(_f_) CLZ((_f_))
#endif // ULIB_BITOP_ENABLE_GENERICS


#if ULIB_BITOP_ENABLE_INLINED_FUNCTIONS
# include "bits.c"
#endif


#endif // ULIB_ENABLE_BITOPS
#endif // _ULIB__H
#ifdef __cplusplus
 }
#endif
