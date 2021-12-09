/*
 * memutils.c
 *
 *  Created on: Dec 8, 2021
 *      Author: Jdany
 */
#include "memutils.h"

void SetMemZero(void* Pointer, Byte Lenght)
{
	while (Lenght-- > 0)
	{
		*((Byte*)Pointer) = 0;
	}
}

