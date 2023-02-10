
#pragma once

/*
    Hand-written set of prerequisite types for other abi headers.
    
    This file includes a subset of prerequisites for <abi_gpio.h>
    For this reason, this file is subject to copyright as in the raspberry pi pico SDK.
    
    See `LICENSE` for details.
    
    SPDX-License-Identifier: BSD-3-Clause
*/


typedef unsigned int uint;

#define _u(x) ((uint) (x))

#define check_hw_layout(type, member, offset) static_assert(offsetof(type, member) == (offset), "hw offset mismatch")
#define check_hw_size(type, size) static_assert(sizeof(type) == (size), "hw size mismatch")

typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;
typedef volatile uint16_t io_rw_16;
typedef const volatile uint16_t io_ro_16;
typedef volatile uint16_t io_wo_16;
typedef volatile uint8_t io_rw_8;
typedef const volatile uint8_t io_ro_8;
typedef volatile uint8_t io_wo_8;

typedef volatile uint8_t *const ioptr;
typedef ioptr const const_ioptr;

// A non-functional (empty) helper macro to help IDEs follow links from the autogenerated
// hardware struct headers in hardware/structs/xxx.h to the raw register definitions
// in hardware/regs/xxx.h. A preprocessor define such as TIMER_TIMEHW_OFFSET (a timer register offset)
// is not generally clickable (in an IDE) if placed in a C comment, so _REG_(TIMER_TIMEHW_OFFSET) is
// included outside of a comment instead
#define _REG_(x)

#define SIO_BASE _u(0xd0000000)
#define PADS_BANK0_BASE _u(0x4001c000)
#define NUM_BANK0_GPIOS 30

#include <prereq/regs_sio.h>
#include <prereq/regs_pads_bank0.h>
#include <prereq/structs_interp.h>
#include <prereq/structs_sio.h>
#include <prereq/structs_padsbank0.h>
