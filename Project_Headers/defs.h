/*
 * defs.h
 *
 *  Created on: Dec 8, 2021
 *      Author: Jdany
 */

#ifndef DEFS_H_
#define DEFS_H_

//
// Library References
//
#include <mc9s08qg8.h>
#include <stddef.h>
#include <stdtypes.h>
#include <limits.h>

//
// Help macro to disable watchdog
//

#define DisableWatchdog _SOPT1.Bits.COPE = 0b0

//
// Utils constants
//
#define NIBBLE_MASK (0x0F)
#define NIBBLE_SIZE (4)

#define BYTE_MASK (0xFF)
#define BYTE_SIZE (8)

#define WORD_MASK (0xFFFF)
#define WORD_SIZE (16)

#define DWORD_MASK (0xFFFFFFFF)
#define DWORD_SIZE (32)

#define INPUT 0x00
#define OUTPUT 0x01
#define ENABLE_PULLUP 0x10
#define DRIVE_STRENGHT 0x20

//
// Util macros
//

#define BV(__bit) (1 << (__bit))

//
// CPU constants
//

#define CPU_FREC	16000000UL
#define BUS_FREC	4000000UL

#define BUS_TICKS_PER_US (BUS_FREC / US_PER_S)
#define CPU_TICKS_PER_US (CPU_FREC / US_PER_S)

#define US_PER_S	1000000
#define US_PER_MS	1000

#endif /* DEFS_H_ */
