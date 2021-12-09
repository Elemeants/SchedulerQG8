/*
 * timers.h
 *
 *  Created on: Dec 8, 2021
 *      Author: Jdany
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include "defs.h"

//
// RTI Delays Constants
//

typedef enum
{
	RTI_DELAY_DISABLE = 0b001,
	RTI_DELAY_8MS = 0b001,
	RTI_DELAY_32MS = 0b010,
	RTI_DELAY_64MS = 0b011,
	RTI_DELAY_128MS = 0b100,
	RTI_DELAY_256MS = 0b101,
	RTI_DELAY_512MS = 0b110,
	RTI_DELAY_1024MS = 0b111
} RTITime_e;

//
// MTI Functions
//

typedef enum
{
	MTI_SRC_BUSCLK = 0b00,
	MTI_SRC_XCLK = 0b01,
	MTI_SRC_TCLK_FALLING = 0b10,
	MTI_SRC_TCLK_RISING = 0b11
} MTIClkSource_e;

//
// Timers functions
//

/**
 * Trains the MTI timer, calculates the less error values for the provided time.
 * 
 * @param ExpectedTime Time in milliseconds to train the MTI.
 * @return Nothing.
 */
extern void MTI_RegTraining(Word ExpectedTime);

/**
 * Initializes the RTI timer that funges as SysTick in this scheluder.
 * 
 * @return Nothing
 */
extern void OS_InitSysTick(void);

/**
 * Initializes the MTI as milli second counter, to help with the delays.
 * 
 * @return Nothing
 */
extern void OS_InitElapsedTime(void);

/**
 * Returns the elapsed time since the OS is init in milli seconds.
 * 
 * @return Milliseconds elapsed since "power on" (OS_Init)
 */
extern LWord OS_GetElapsedTime(void);

#endif /* TIMERS_H_ */
