#pragma once
#include <Lib/Types.h>
#include <Kernel/System/Debug.h>

EXTC
{
	typedef struct
	{
		uint16_t DI, SI, BP, SP, BX, DX, CX, AX;
		uint16_t GS, FS, ES, DS, EFlags;
	} PACKED Registers16;

	extern void CallInterrupt16(uint8_t interrupt, Registers16* regs);
}