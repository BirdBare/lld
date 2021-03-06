//
//
//
//
//



#ifndef CLOCK_H
#define CLOCK_H

#include "board.h"
#include "rcc_lld.h"

extern volatile uint32_t CLOCK_SPEED[5];
//Clock Speeds for APB1, APB2, AHB, CPU, USB clocks in that order in Mhz.
//AHB max is cpu max, APB1 max is AHB max / 4. APB2 max is AHB max / 2.
extern volatile uint16_t CLOCK_PRESCALER[2];
//Clock Prescalers for APB1, APB2
#define APB1 0
#define APB2 1
#define AHB 2
#define CPU 3
#define USB 4

#ifndef HSI_SPEED
	#error "HSI_SPEED MISSING. V2 Clock.h" 
#endif
#ifndef USB_SPEED
	#error "USB_SPEED MISSING. V2 Clock.h"
#endif
#ifndef VCO_MAX
	#error "VCO_MAX MISSING. V2 Clock.h"
#endif
#ifndef VCO_MIN
	#error "VCO_MIN MISSING. V2 Clock.h"
#endif
#ifndef CPU_MAX
	#error "CPU_MAX MISSING. V2 Clock.h"
#endif
#ifndef AHB_MAX
	#error "AHB_MAX MISSING. V2 Clock.h"
#endif
#ifndef APB1_MAX
	#error "APB1_MAX MISSING. V2 Clock.h"
#endif
#ifndef APB2_MAX 
	#error "APB2_MAX MISSING. V2 Clock.h"
#endif
//Board CONSTANTS

struct ClockConfig
{
	uint32_t cpu_speed; //speed of the microcontroller in hz
	uint32_t ahb_speed; //speed of the hardware bus in hz
	uint32_t apb1_speed; //speed of low speed peripheral bus in hz
	uint32_t apb2_speed; //speed of high speed peripheral bus in hz

	uint32_t hse_speed; //speed of external clock in hz. if none is used. keep zero
};


//******************************************************************************
//
//
//
//******************************************************************************
static inline uint32_t LldClockInit(void)
{
	return 0;
}

//******************************************************************************
//
//
//
//******************************************************************************
static inline uint32_t LldClockDeinit(void)
{
	return 0;
}

//******************************************************************************
//	
//										 
//	
//******************************************************************************
uint32_t LldClockConfig(
const struct ClockConfig * const clock_config);


//******************************************************************************
//
//
//
//******************************************************************************
uint32_t LldClockResetConfig(void);


//******************************************************************************
//	
//										 
//	
//******************************************************************************
ALWAYS_INLINE uint32_t LldClockGetSpeed(uint32_t bus)
{
	return CLOCK_SPEED[bus];
}
#define LldClockGetCpuSpeed(void) LldClockGetSpeed(CPU);


//******************************************************************************
//	
//										 
//	
//******************************************************************************
ALWAYS_INLINE uint32_t LldClockGetPeripheralSpeed(
	const struct RccObject * const rcc_object)
{
	return LldClockGetSpeed(rcc_object->peripheral_bus);
}

//******************************************************************************
ALWAYS_INLINE uint32_t LldClockGetPeripheralPrescaler(
	const struct RccObject * const rcc_object)
{
	return CLOCK_PRESCALER[rcc_object->peripheral_bus];
}




#endif
