//
//
//
//
//

#include "main.h"
#include "gpio_hal.h"
#include "spi_hal.h"
#include "nvic_lld.h"
#include "nokia5110.h"
#include "timer_hal.h"
#include "clock_hal.h"
#include "bareos.h"

void NMI_Handler(void)
{
BREAK(99);
}

void HardFault_Handler(void)
{
BREAK(98);
}

void MemManage_Handler(void)
{
BREAK(97);
}

void BusFault_Handler(void)
{
BREAK(96);
}

void UsageFault_Handler(void)
{
BREAK(95);
}
















uint8_t thread3_mem[500];
void thread3(void *args)
{
GpioInit(&GPIOD_OBJECT);

	struct GpioConfig gpio_config = {0};
	//pin config struct

	gpio_config.pin = PIN_12;
	gpio_config.mode = MODE_OUTPUT;
	gpio_config.speed = SPEED_VHIGH;
	GpioConfig(&GPIOD_OBJECT, &gpio_config);
	//config Green LED

	float time = 1000.15444562;

	while(1)
	{
		time += (float) BareOSTimerGetTime() - 50.123552;
		
		if(time > 23297.05)
			GpioToggleOutput(&GPIOD_OBJECT, PIN_12);
		BareOSTimerDelayInterrupt(250);
	}
}

uint8_t thread4_mem[500];
void thread4(void *args)
{

GpioInit(&GPIOD_OBJECT);

	struct GpioConfig gpio_config = {0};
	//pin config struct

	gpio_config.pin = PIN_15;
	gpio_config.mode = MODE_OUTPUT;
	gpio_config.speed = SPEED_VHIGH;
	GpioConfig(&GPIOD_OBJECT, &gpio_config);
	//config Blue LED

	while(1)
	{
			GpioToggleOutput(&GPIOD_OBJECT, PIN_15);
		BareOSTimerDelayPolled(500);
	}
}







uint8_t blink_memory[500]; 
void blink(void *args)
{
	GpioInit(&GPIOD_OBJECT);

	struct GpioConfig gpio_config = {0};
	//pin config struct

	gpio_config.pin = PIN_14;
	gpio_config.mode = MODE_OUTPUT;
	gpio_config.speed = SPEED_VHIGH;
	GpioConfig(&GPIOD_OBJECT, &gpio_config);
	//config Red LED

	while(1)
	{
			GpioToggleOutput(&GPIOD_OBJECT, PIN_14);
		BareOSTimerDelayInterrupt(750);
	}
};



uint8_t spi_memory[500]; 
void spi(void *args)
{

GpioInit(&GPIOA_OBJECT);

	NvicEnableInterrupt(SPI1_IRQn);

	struct GpioConfig gpio_config = {0};
	//pin config struct

	gpio_config.pin = PIN_5 | PIN_7 | PIN_6;
	gpio_config.mode = MODE_ALTERNATE;
	gpio_config.speed = SPEED_VHIGH;
	gpio_config.alternate = ALTERNATE_5;
	gpio_config.pupd = PUPD_PD;
	GpioConfig(&GPIOA_OBJECT, &gpio_config);
	//config spi pins.
	//SPI EXPERIMENTAL

	SpiInit(&SPI1_OBJECT);
	//init spi

	struct SpiConfig spi_config = 
	{ .clock_frequency = 300000,
		.slave_gpio_object = &GPIOA_OBJECT,
		.slave_gpio_pin = PIN_6,
		.interrupt = &Nokia5110Interrupt,
		.interrupt_args = &nokia};

	SpiConfigMaster(&SPI1_OBJECT, &spi_config);
	//config spi1 for lowest clock speed and default settings



	uint8_t data_out[5] = {0xff,0xf,0xb,0xe,3};

	while(1)
	{
		LldSpiTransferInterrupt(&SPI1_OBJECT,data_out,data_out, 1);
		BareOSTimerDelayInterrupt(10);
		SpiTransferInterrupt(&SPI1_OBJECT,data_out,data_out, 1);
		BareOSTimerDelayPolled(789);
	}
}








int main(void)
{

//#####BAREOS INIT##########

	SCB->CPACR |= 0b1111 << 20;
	FPU->FPCCR |= 0b11 << 30;
	//enable fpu, lazy stacking, etc.
	
	asm volatile("DSB");
	asm volatile("ISB");
	
	struct ClockConfig clock_config = 
	{168000000,
	168000000,
	42000000,
	84000000,
	8000000};
	ClockConfig(&clock_config);
	//configure the cpu clocks

	BareOSSchedulerInit(1000,0);
	//init system
	
	NvicEnableInterrupt(TIM6_DAC_IRQn);

	struct TimerConfig system_config = {.tick_frequency = 10000};
	BareOSTimerInit(&TIMER6_OBJECT,&system_config);
	BareOSTimerStart();
	//initalize system timer


	
//######END BAREOS INIT##########


	struct BareOSThread *blink_thread =	
		BareOSThreadCreateThread(blink_memory,&blink,0,500);

	struct BareOSThread *spi_thread =	
		BareOSThreadCreateThread(spi_memory,&spi,0,500);

	struct BareOSThread *thread3_p =	
		BareOSThreadCreateThread(thread3_mem,&thread3,0,500);

	struct BareOSThread *thread4_p =	
		BareOSThreadCreateThread(thread4_mem,&thread4,0,500);

BareOSSchedulerAddThread(blink_thread);
BareOSSchedulerAddThread(spi_thread);
BareOSSchedulerAddThread(thread3_p);
BareOSSchedulerAddThread(thread4_p);


	
//
//############################## END SYSTEM INIT @@@@@@@####################
//

		while(1)
	{
		BareOSTimerDelayPolled(1000);
	}
	
	return 1;
}

