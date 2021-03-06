//
//
//
//
//


#include "rcc_lld.h"

//******************************************************************************
//	
//										 
//	
//******************************************************************************
uint32_t RccEnableClock(const struct RccObject * const rcc_object)
{
	uint32_t * const clockreg = (uint32_t *)(void *)((uint32_t)RCC + rcc_object->reg_offset);
	//get clock reg address

	uint32_t setbit = 1 << rcc_object->bit_offset;
	//get bit

	uint32_t prevstate = *clockreg & setbit;
	//get previous state

	*clockreg |= setbit;
	//enable clock bit
	
	return prevstate;
}

//******************************************************************************
//	
//										 
//	
//******************************************************************************
void RccDisableClock(const struct RccObject * const rcc_object)
{
	uint32_t * const clockreg = (uint32_t *)(void *)((uint32_t)RCC + rcc_object->reg_offset);
	//get clock reg address

	uint32_t resetbit = ~(1 << rcc_object->bit_offset);
	//get bit

	*clockreg &= resetbit;
	//disable clock bit
}

//******************************************************************************
//	
//										 
//	
//******************************************************************************
void RccResetPeripheral(const struct RccObject * const rcc_object)
{
	uint32_t *resetreg = (uint32_t *)(void *)((uint32_t)RCC + rcc_object->reg_offset - 0x20);
	//get clock reg address

	uint32_t bit = 1 << rcc_object->bit_offset;
	//get bit

	*resetreg |= bit;
	//enable reset bit

	for(int i = 10; i != 0; i--)
	{
		asm volatile("");
	}
	//wait for 35 ish clock cycles to let things reset before turning proceding.

	*resetreg &= ~bit;
	//disable reset bit
}

