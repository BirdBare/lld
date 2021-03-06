//
//
//
//
//



#ifndef STM32F767_DMA_H
#define STM32F767_DMA_H

#include "board.h"
#include "rcc_lld.h"
#include "nvic_lld.h"
#include "clock_lld.h"
#include "mutex.h"

//to get DMA Base address. Take DMA_Stream & ~255.

struct DmaObject
{
#ifdef USE_BAREOS
	struct Mutex mutex;

	uint16_t initialized:1;
	uint16_t:15;
#endif

	const struct RccObject rcc;
	const struct NvicObject nvic;

	const uint8_t flag_register_offset;
	const uint8_t flag_offset;

	volatile DMA_Stream_TypeDef * const dma;

	void (*callback)(void *args);
	void *args;

}; 

extern struct DmaObject 
	DMA1S0_OBJECT,
	DMA1S1_OBJECT,
	DMA1S2_OBJECT,
	DMA1S3_OBJECT,
	DMA1S4_OBJECT,
	DMA1S5_OBJECT,
	DMA1S6_OBJECT,
	DMA1S7_OBJECT,

	DMA2S0_OBJECT,
	DMA2S1_OBJECT,
	DMA2S2_OBJECT,
	DMA2S3_OBJECT,
	DMA2S4_OBJECT,
	DMA2S6_OBJECT,
	DMA2S7_OBJECT;

struct DmaConfig
{
	union
	{
		void *from_address; //address data. use in all other functions
		uint32_t value_to_set; //used in LldDmaSetMemory Function
	};

	void *to_address;

	uint32_t num_data;

	void (*callback)(void *args);
	void *args;

	union
	{
		uint32_t cr;

		struct
		{
		//LSB
			uint32_t:1;
			uint32_t data_size:2; 
			uint32_t half_transfer_callback:1; //call callback at half transfer
																				//instead of end of transfer
			uint32_t:4;
			uint32_t circular:1; //circular buffer mode
			uint32_t:7;
			uint32_t priority:2;
			uint32_t double_buffer:1;
			uint32_t:6;
			uint32_t channel:3;
			uint32_t:4;
		//MSB
		};
	};

};
#define DMA_ISR_FEIF 0b1
#define DMA_ISR_DMEIF 0b100
#define DMA_ISR_TEIF 0b1000
#define DMA_ISR_HTIF 0b10000
#define DMA_ISR_TCIF 0b100000


static inline uint32_t LldDmaInit(struct DmaObject *dma_object)
{
	NvicEnableObjectInterrupt(&dma_object->nvic);
	RccEnableClock(&dma_object->rcc);
	return 0;
}

static inline uint32_t LldDmaDeinit(struct DmaObject *dma_object)
{
	NvicDisableObjectInterrupt(&dma_object->nvic);
	RccDisableClock(&dma_object->rcc);
	return 0;
}

static inline uint32_t LldDmaResetConfig(struct DmaObject *dma_object)
{
	return 0;
}

//******************************************************************************
//	
//										Dma Get/Clear flags 
//	
//******************************************************************************

static uint32_t LldDmaGetFlags(struct DmaObject *dma_object)
{
	volatile DMA_Stream_TypeDef *dma_stream = dma_object->dma;
	//get dma stream

	volatile DMA_TypeDef *dma = (DMA_TypeDef *)((uint32_t)dma_stream & ~255);
	//get dma


	uint32_t *flag_register = (uint32_t *)((uint32_t)&dma->LISR + 
		dma_object->flag_register_offset);


	return 0b111101 & (*flag_register >> dma_object->flag_offset);
	//return the flag register value
}

static void LldDmaClearFlags(struct DmaObject *dma_object, uint32_t flags)
{
	volatile DMA_Stream_TypeDef *dma_stream = dma_object->dma;
	//get dma stream

	volatile DMA_TypeDef *dma = (DMA_TypeDef *)((uint32_t)dma_stream & ~255);
	//get dma

	uint32_t *flag_register = (uint32_t *)((uint32_t)&dma->LIFCR + 
		dma_object->flag_register_offset);

		*flag_register = (0b111101 & flags) << dma_object->flag_offset;
	//return the flag register value
}
//##############################################################################





//******************************************************************************
//
//
//
//******************************************************************************
static uint32_t LldDmaStartM2P(struct DmaObject *dma_object,
	struct DmaConfig *dma_config)
{
	LldDmaClearFlags(dma_object, 0b111101);
	//clear flags first

	volatile DMA_Stream_TypeDef *dma = dma_object->dma;
	//dma variable

	dma->NDTR = dma_config->num_data;
	//set num data.

	dma->PAR = (uint32_t)dma_config->to_address;
	//should be peripheral data register

	dma->M0AR = (uint32_t)dma_config->from_address;
	//should be memory area with data to send

	dma->FCR = DMA_SxFCR_FEIE;
	//set error interrupt

	uint32_t cr = dma_config->cr;

	if(dma_config->callback != 0 && dma_config->half_transfer_callback == 0)
	{
		cr |= DMA_SxCR_TCIE;
	}

	dma_object->callback = dma_config->callback;
	dma_object->args = dma_config->args;

	dma->CR = cr | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | DMA_SxCR_DIR_0 | 
		dma_config->data_size << 11 | dma_config->data_size << 13 | DMA_SxCR_MINC | 
		DMA_SxCR_EN; 
	//set error interrupts and other needed stuff

	return 0;
}




//******************************************************************************
//
//
//
//******************************************************************************
static uint32_t LldDmaStartP2M(struct DmaObject *dma_object,
	struct DmaConfig *dma_config)
{
	LldDmaClearFlags(dma_object, 0b111101);
	//clear flags first

	volatile DMA_Stream_TypeDef *dma = dma_object->dma;
	//dma variable

	dma->NDTR = dma_config->num_data;
	//set num data.

	dma->PAR = (uint32_t)dma_config->from_address;
	//should be peripheral data register

	dma->M0AR = (uint32_t)dma_config->to_address;
	//should be memory area with data to send

	dma->FCR = DMA_SxFCR_FEIE;
	//set error interrupt

	uint32_t cr = dma_config->cr;

	if(dma_config->callback != 0 && dma_config->half_transfer_callback == 0)
	{
		cr |= DMA_SxCR_TCIE;
	}

	dma_object->callback = dma_config->callback;
	dma_object->args = dma_config->args;

	dma->CR = cr | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | dma_config->data_size << 11 | 
		dma_config->data_size << 13 | DMA_SxCR_MINC | DMA_SxCR_EN;
	//set error interrupts and other needed stuff

	return 0;
}

static uint32_t LldDmaTransfer(struct DmaObject *dma_object,
	struct DmaConfig* dma_config)
{
	LldDmaClearFlags(dma_object, 0b111101);
	//clear flags first

	volatile DMA_Stream_TypeDef *dma = dma_object->dma;
	//dma variable

	dma->NDTR = dma_config->num_data;
	//set num data.

	dma->PAR = (uint32_t)dma_config->from_address;
	//should be peripheral data register

	dma->M0AR = (uint32_t)dma_config->to_address;
	//should be memory area with data to send

	dma->FCR = DMA_SxFCR_FEIE | DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 | DMA_SxFCR_FTH_1;
	//set error interrupt

	uint32_t cr = dma_config->cr;

	if(dma_config->callback != 0 && dma_config->half_transfer_callback == 0)
	{
		cr |= DMA_SxCR_TCIE;
	}

	dma_object->callback = dma_config->callback;
	dma_object->args = dma_config->args;

	dma->CR = cr | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | dma_config->data_size << 11 | 
		dma_config->data_size << 13 | DMA_SxCR_MINC | DMA_SxCR_PINC | 
		DMA_SxCR_DIR_1 | DMA_SxCR_MBURST_0 | DMA_SxCR_PBURST_0 | DMA_SxCR_EN;
	//set error interrupts and other needed stuff

	return 0;
}


static uint32_t LldDmaSetMemory(struct DmaObject *dma_object,
	struct DmaConfig* dma_config)
{
	LldDmaClearFlags(dma_object, 0b111101);
	//clear flags first

	volatile DMA_Stream_TypeDef *dma = dma_object->dma;
	//dma variable

	dma->NDTR = dma_config->num_data;
	//set num data.

	dma->PAR = (uint32_t)&dma_config->value_to_set;
	//should be peripheral data register

	dma->M0AR = (uint32_t)dma_config->to_address;
	//should be memory area with data to send

	dma->FCR = DMA_SxFCR_FEIE | DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 | DMA_SxFCR_FTH_1;
	//set error interrupt

	uint32_t cr = dma_config->cr;

	if(dma_config->callback != 0 && dma_config->half_transfer_callback == 0)
	{
		cr |= DMA_SxCR_TCIE;
	}

	dma_object->callback = dma_config->callback;
	dma_object->args = dma_config->args;

	dma->CR = cr | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | dma_config->data_size << 11 | 
		dma_config->data_size << 13 | DMA_SxCR_MINC | DMA_SxCR_DIR_1 | 
		DMA_SxCR_MBURST_0 | DMA_SxCR_PBURST_0 | DMA_SxCR_EN;
	//set error interrupts and other needed stuff

	return 0;
}












#endif
