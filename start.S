#
# Copyright (C) 2025 Sylas Hollander.
# PURPOSE: ASM entry point for Apple TV.
# SPDX-License-Identifier: MIT
#

.extern _atvlib_init

.text

.global start
start:
    # Push bootArgs pointer to the stack.
    push %eax
    # Call C entry point
    call _atvlib_init
    # Halt the CPU
    hlt

.global _halt
_halt:
    cld
    hlt
    jmp _halt