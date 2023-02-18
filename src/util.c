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
// util.c
// Utility functions
// NOTES:
//
//
#ifdef __cplusplus
 extern "C" {
#endif

#include "debug.h"
#include "util.h"

#if ULIB_ENABLE_UTIL

#include <string.h>

void mem_init(void *mem, uint8_t value, uint32_t size) {
#if DO_UTIL_SAFETY_CHECKS
	if (mem == NULL) {
		return;
	}
#endif

	memset(mem, value, size);
	return;
}

#endif // ULIB_ENABLE_UTIL
#ifdef __cplusplus
 }
#endif
