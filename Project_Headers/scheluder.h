/*
 * scheluder.h
 *
 *  Created on: Dec 8, 2021
 *      Author: Jdany
 */

#ifndef SCHELUDER_H_
#define SCHELUDER_H_

#include "defs.h"
#include "timers.h"
#include "memutils.h"

//
// Types definitions
//

/**
 * Function type for threads
 */
typedef void (*Thread_f)(void);

/**
 * Fancy way for a void pointer
 */
typedef void* Pointer_t;

//
// Structures definitions
//

/**
 * Represents the stack memory of the threads.
 */
struct _StackMemory_t
{
	// Lower address memory address (ex: 0x100)
	Pointer_t StartPtr;
	// Higher address memory address (ex: 0x200)
	Pointer_t EndPtr;
	// Size of the stack
	Byte Size;
};

/**
 * Represents the data of the thread.
 */
struct _OsThread_t
{
	// Function call for the thread.
	Thread_f Thread;
	// Current SP of the thread (used in context switching)
	Pointer_t StackPointer;
	// Stack pointer initial configuration
	struct _StackMemory_t InitialSPConfig;

	// Next thread to be executed
	struct _OsThread_t *NextThread;
	// Previous thread
	struct _OsThread_t *PrevThread;

	struct
	{
		// Is the current thread executed?
		Byte IsRunnig :1;
		Byte :7;
	} Status;
};

/**
 * Global OS context.
 */
struct _OSContext_t
{
	// Elapsed SysTicks
	LWord Ticks;
	// Elapsed Milliseconds
	LWord MsElapsed;

	struct 
	{
		// Thread currently executed
		struct _OsThread_t *CurrentThread;
	} Scheluder;
};

typedef struct _OsThread_t OsThread_t;
typedef struct _OSContext_t OSContext_t;
typedef struct _StackMemory_t StackMemory_t;

//
// Macros helpers
//

/**
 * Macro to help in the definition of threads.
 * 
 * Allocates an array of the stack size and defines the initial
 * OsThread_t structure with the current stack pointer and the
 * function to be executed.
 * 
 * Examples:
 * 	DEFINE_THREAD(thMain, 128U, _thMainFunction)
 * 	{
 * 		EnableInterrupts
 * 		
 * 		// Startup code
 * 		
 * 		for (;;)
 * 		{
 * 			// Loop code
 * 		}
 * 	}
 * 
 */
#define DEFINE_THREAD(__thread, __size, __function)	\
	void __function (void); 						\
	static Byte __thread##_stack_mem[__size]; 		\
	const Byte __thread##_stack_size = __size;  	\
	static OsThread_t __thread = {					\
			__function,								\
			(__thread##_stack_mem + __size), 		\
			{										\
					(__thread##_stack_mem + __size),\
					__thread##_stack_mem,			\
					__size							\
			},										\
			NULL,									\
			NULL,									\
			0x00									\
	};												\
	void __function (void)

//
// Function definitions
//

/**
 * Initializes the Scheluder with the provided threads.
 * The scheluder uses Round-robin so, it starts with the
 * first added thread and after each SysTick the scheluder
 * goes to the next added thread and so on, looping endless
 * 
 * Initializes the timers and interruption needed.
 * 
 * WARNING: This function never returns.
 */
extern void OS_Init(void);

/**
 * This function adds a thread to the scheluder list, the
 * structure must be defined with the thread configuration.
 * 
 * Example:
 * 	OS_AddThread(&thMain);
 * 	
 * @param Thread The thread to be added
 * @return Nothing
 */
extern void OS_AddThread(OsThread_t *Thread);

/**
 * Uses the Elapsed time counter, to create a ms delay.
 * 
 * @param DelayMs Delay in milliseconds to be executed.
 * @return Nothing
 */
extern void OS_DelayMs(Word DelayMs);

#endif /* SCHELUDER_H_ */
