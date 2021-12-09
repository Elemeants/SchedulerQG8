#include "scheluder.h"

//
// Defs internal methods of the scheluder
//

/**
 * [Private] Switchs the OS to the next context (thread)
 * 
 * @param Thread Current thread been executed.
 * @return Nothing 
 */
static void OS_SwitchContext(OsThread_t* Thread);

/**
 * [Private] Used to initialize the first "context switch"
 * filling the "stack pointer" with the needed data to do
 * the switch.
 * 
 * @param Thread Current thread been executed.
 * @return Nothing 
 */
static void OS_PullFirstContext(OsThread_t* Thread);

/**
 * [Private] Before initialize the OS, this method sets the
 * "NextThread" and "PrevThread" attributes for each thread
 * setting the execution order.
 * 
 * And sets the initial thread on the scheluder.
 * 
 * @return Nothing
 */
static void OS_InitScheluder(void);

/**
 * [Private] OsContext variable
 */
static OSContext_t OsContext;

//
// Public methods of the scheluder
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
void OS_Init(void)
{
	//
	// Initialize the MTI Register
	//
	OS_InitElapsedTime();

	//
	// Initialize the executing order
	//
	OS_InitScheluder();
	
	//
	// Initialize the RTI interrupt as SysTick
	//
	OS_InitSysTick();

	//
	// Initialize the scheluder switching to the first thread.
	//
	OS_PullFirstContext(OsContext.Scheluder.CurrentThread);

	/* The program should never return here */
	while (TRUE)
		;
}

/**
 * Temporal macro to help to push values into the stack pointer of the thread.
 * Is only defined inside OS_AddThread
 */
#define PUSH_TO_SP(__sp, __value) { ((Byte*)__sp)--; *((Byte*)__sp) = __value; }

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
void OS_AddThread(OsThread_t *Thread)
{
	// Initialize Stack for new Thread
	PUSH_TO_SP(Thread->StackPointer, (Byte)Thread->Thread);
	// PCL
	PUSH_TO_SP(Thread->StackPointer, (Byte)((Word)Thread->Thread >> 8));
	// PCH
	PUSH_TO_SP(Thread->StackPointer, 0x00);
	// A
	PUSH_TO_SP(Thread->StackPointer, 0x6A);
	// CCR | Default value is 0x6A
	PUSH_TO_SP(Thread->StackPointer, 0x00);
	// X
	PUSH_TO_SP(Thread->StackPointer, 0x00);
	// H

	if (!OsContext.Scheluder.CurrentThread)
	{
		OsContext.Scheluder.CurrentThread = Thread;
	}
	else
	{
		OsContext.Scheluder.CurrentThread->NextThread = Thread;
		Thread->PrevThread = OsContext.Scheluder.CurrentThread;
		OsContext.Scheluder.CurrentThread = Thread;
	}
}

#undef PUSH_TO_SP

/**
 * Uses the Elapsed time counter, to create a ms delay.
 * 
 * @param DelayMs Delay in milliseconds to be executed.
 * @return Nothing
 */
void OS_DelayMs(Word DelayMs)
{
	LWord StartMs = OsContext.MsElapsed;
	while ((OsContext.MsElapsed - StartMs) < DelayMs)
		;
}

//
// Timers functions
//

/**
 * Trains the MTI timer, calculates the less error values for the provided time.
 * 
 * @param ExpectedTime Time in milliseconds to train the MTI.
 * @return Nothing.
 */
void MTI_RegTraining(Word ExpectedTime)
{
	Word CurrentError = 0;
	Word LowestError = UINT_MAX;

	Byte OptimalModule = UCHAR_MAX;
	Byte Module = UCHAR_MAX;

	Byte PrescaleLogical = 0;
	Byte PrescaleLinear = 0;

	PrescaleLinear =
			(Byte) ((ExpectedTime * (Word) BUS_TICKS_PER_US)/ BYTE_MASK);
	for (PrescaleLogical = 0;
			(PrescaleLogical & NIBBLE_MASK)< BYTE_SIZE; PrescaleLogical ++) {
		if ((1 << (PrescaleLogical & NIBBLE_MASK))& PrescaleLinear)
		{
			PrescaleLogical = (PrescaleLogical & NIBBLE_MASK)
					| (PrescaleLogical << NIBBLE_SIZE);
		}
	}
	PrescaleLogical >>= NIBBLE_SIZE;
	PrescaleLogical++;

	PrescaleLinear = 1 << PrescaleLogical;

	do
	{
		CurrentError = ((PrescaleLinear * Module) / (Word) BUS_TICKS_PER_US);
		CurrentError =
				CurrentError < ExpectedTime ?
						(ExpectedTime - CurrentError) :
						(CurrentError - ExpectedTime);
		if (CurrentError < LowestError)
		{
			LowestError = CurrentError;
			OptimalModule = Module;
		}

	} while (Module-- > 0);

	_MTIMMOD.Byte = OptimalModule;
	_MTIMCLK.MergedBits.grpPS = PrescaleLogical;
}

/**
 * Initializes the RTI timer that funges as SysTick in this scheluder.
 * 
 * @return Nothing
 */
void OS_InitSysTick(void)
{
	_SRTISC.MergedBits.grpRTIS = (Byte) RTI_DELAY_8MS;
	_SRTISC.Bits.RTIE = TRUE;
}

/**
 * Initializes the MTI as milli second counter, to help with the delays.
 * 
 * @return Nothing
 */
void OS_InitElapsedTime(void)
{
	_MTIMSC.Bits.TSTP = 0b1;
	_MTIMCLK.MergedBits.grpCLKS = (Byte) MTI_SRC_BUSCLK;

	// Train the MTI registers to 1ms
	MTI_RegTraining(1000);

	// Enable the interrupt
	_MTIMSC.Bits.TOIE = TRUE;
	_MTIMSC.Bits.TSTP = 0b0;
}

/**
 * Returns the elapsed time since the OS is init in milli seconds.
 * 
 * @return Milliseconds elapsed since "power on" (OS_Init)
 */
LWord OS_GetElapsedTime(void)
{
	return OsContext.MsElapsed;
}

interrupt VectorNumber_Vrti void RTI_IRQ(void)
{
	_SRTISC.Bits.RTIACK = TRUE;
	_SRTISC.Bits.RTIE = TRUE;

	OsContext.Ticks++;

	OS_SwitchContext(OsContext.Scheluder.CurrentThread);
}

interrupt VectorNumber_Vmtim void MTI_IRQ(void)
{
	_MTIMSC.Bits.TOF = 0b0;

	/* Increment OS microseconds timer */
	OsContext.MsElapsed++;

	_MTIMSC.Bits.TSTP = 0b0;
	_MTIMSC.Bits.TRST = 0b1;
}

//
// Implementation internal methods of the scheluder
//

/**
 * [Private] Before initialize the OS, this method sets the
 * "NextThread" and "PrevThread" attributes for each thread
 * setting the execution order.
 * 
 * And sets the initial thread on the scheluder.
 * 
 * @return Nothing
 */
void OS_InitScheluder(void)
{
	OsThread_t *LastThread = OsContext.Scheluder.CurrentThread;
	if (!OsContext.Scheluder.CurrentThread)
	{
		return;
	}
	// Close ring linked thread list
	while (LastThread->NextThread)
		LastThread = LastThread->NextThread;

	while (OsContext.Scheluder.CurrentThread->PrevThread)
		OsContext.Scheluder.CurrentThread =
				OsContext.Scheluder.CurrentThread->PrevThread;
	LastThread->NextThread = OsContext.Scheluder.CurrentThread;
	OsContext.Scheluder.CurrentThread->PrevThread = LastThread;
}

/**
 * [Private] Used to initialize the first "context switch"
 * filling the "stack pointer" with the needed data to do
 * the switch.
 * 
 * @param Thread Current thread been executed.
 * @return Nothing 
 */
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME
#pragma NO_RETURN
void OS_SwitchContext(OsThread_t* Thread)
{
	// We have stored the 
	asm
	{
		PSHA	// Store A
		TPA// Store CCR
		PSHA
		PSHX// Store X
		PSHH// Store H

		// Update thread SP
		TSX
		PSHH
		PSHX

		LDHX 3,SP

		PULA
		STA OsThread_t.StackPointer + 1, X
		PULA
		STA OsThread_t.StackPointer, X

		// Change context
		LDHX OsThread_t.NextThread, X
		STHX OsContext.Scheluder
		LDHX OsThread_t.StackPointer, X
		TXS

		PULH// Load H
		PULX// Load X
		PULA// Load CCR
		TAP
		PULA// Load A

		RTS
	};
}

/**
 * [Private] Switchs the OS to the next context (thread)
 * 
 * @param Thread Current thread been executed.
 * @return Nothing 
 */
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME
#pragma NO_RETURN
void OS_PullFirstContext(OsThread_t* Thread)
{
	// Have the Thread Ptr on H:X registers
	asm
	{
		// Load new StackPointer from H:X
		LDHX OsThread_t.StackPointer, X
		TXS

		// Because the stack already contains the push of the data.
		// Read entry point of Thread and load to H:X
		PULH// Load H
		PULX// Load X
		PULA// Load CCR
		TAP
		PULA// Load A

		// In the stack only remains the entry point
		// So RTS will return to the thread entry point

		CLI
		// Return to entry point
		RTS
	};
}

