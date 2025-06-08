/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: General purpose video functions for linear framebuffers
 * SPDX-License-Identifier: MIT
*/

#pragma once

typedef char                int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef int                 int32_t;
typedef unsigned int        uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;
typedef _Bool               boolean_t;
typedef __SIZE_TYPE__       size_t;
typedef __UINTPTR_TYPE__    uintptr_t;

typedef unsigned short      wchar_t;
typedef uint32_t            uintn_t;

#define true                1
#define false               0
#define NULL                (void *) 0

#define noreturn            _Noreturn