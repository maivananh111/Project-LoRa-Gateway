/*
 * dma.cpp
 *
 *  Created on: Jan 13, 2023
 *      Author: anh
 */


#include "peripheral_config.h"
#if ENABLE_DMA

#include "periph/dma.h"

#include "sdkconfig.h"
#include "periph/systick.h"
#if CONFIG_USE_LOG_MONITOR && DMA_LOG
#include "system/log.h"
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */


#if CONFIG_USE_LOG_MONITOR && DMA_LOG
static const char *TAG = "DMA";
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
static const uint8_t Channel_Index[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};

dma::dma(DMA_TypeDef *dma){
	_dma = dma;
}

void dma::ClearIFCR(__IO uint32_t Value){
	(Stream < 4)? (_dma -> LIFCR = Value) : (_dma -> HIFCR = Value);
}
void dma::ClearAllIntrFlag(void){
	ClearIFCR((0x3FU << _Intr_Index));
}

__IO uint32_t dma::GetISR(void){
	__IO uint32_t isr = 0;
	(Stream < 4)? (isr = _dma -> LISR) : (isr = _dma -> HISR);
	return isr;
}

stm_ret_t dma::init(dma_config_t *conf){
	stm_ret_t ret;
	__IO uint32_t tmpreg;
	_conf = conf;
#if defined(DMA1)
	if(_dma == DMA1) RCC -> AHB1ENR |= RCC_AHB1ENR_DMA1EN;
#endif /* defined(DMA1) */
#if defined(DMA2)
	if(_dma == DMA2) RCC -> AHB1ENR |= RCC_AHB1ENR_DMA2EN;
#endif /* defined(DMA2) */
	_conf -> stream -> CR &=~ DMA_SxCR_EN;
	ret = wait_flag_in_register_timeout(&(_conf -> stream -> CR), DMA_SxCR_EN, FLAG_RESET, 500U);
	if(is_timeout(&ret)){
		set_return_line(&ret, __LINE__);
		return ret;
	}

	tmpreg = (_conf -> stream) -> CR;
	tmpreg &= ~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST |
				DMA_SxCR_CT    | DMA_SxCR_DBM    | DMA_SxCR_PL     |
			    DMA_SxCR_MSIZE | DMA_SxCR_PSIZE  | DMA_SxCR_MINC   |
			    DMA_SxCR_PINC  | DMA_SxCR_CIRC   | DMA_SxCR_DIR    |
			    DMA_SxCR_PFCTRL| DMA_SxCR_TCIE   | DMA_SxCR_HTIE   |
				DMA_SxCR_TEIE  | DMA_SxCR_DMEIE  | DMA_SxCR_EN     );

	tmpreg |= (uint32_t)((_conf->channel << DMA_SxCR_CHSEL_Pos) | (_conf->channelpriority << DMA_SxCR_PL_Pos) |
						 (_conf->datasize << DMA_SxCR_PSIZE_Pos) | (_conf->datasize << DMA_SxCR_MSIZE_Pos)  |
						 DMA_SxCR_MINC | (_conf->mode << DMA_SxCR_CIRC_Pos) | (_conf->direction << DMA_SxCR_DIR_Pos));

	if(_conf->fifo == DMA_FIF0) {tmpreg |= (_conf->burst << DMA_SxCR_MBURST_Pos) | (_conf->burst << DMA_SxCR_PBURST_Pos);}
	(_conf -> stream) -> CR = tmpreg;

	tmpreg = (_conf -> stream) -> FCR;
	tmpreg &=~ (DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);
	tmpreg |= _conf -> fifo;
	if(_conf -> fifo == DMA_FIF0) tmpreg |= DMA_SxFCR_FTH;
	_conf -> stream -> FCR = tmpreg;

	Stream = (((uint32_t)_conf -> stream & 0xFFU) - 16U) / 24U;
	_Intr_Index = Channel_Index[Stream];

	ClearAllIntrFlag();

	if(_dma == DMA1){
		if(Stream == 7U) _IRQn = DMA1_Stream7_IRQn;
		else _IRQn = (IRQn_Type)(Stream + 11U);
	}
	else if(_dma == DMA2){
		if(Stream > 4U) _IRQn = (IRQn_Type)(Stream + 63U);
		else _IRQn = (IRQn_Type)(Stream + 56U);
	}

	if(_conf -> interruptpriority < CONFIG_RTOS_MAX_SYSTEM_INTERRUPT_PRIORITY){
		set_return(&ret, STM_ERR, __LINE__);
#if CONFIG_USE_LOG_MONITOR && DMA_LOG
		LOG_ERROR(TAG, "%s -> %s -> Invalid priority, please increase the priority value.", __FILE__, __FUNCTION__);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
#if CONFIG_FAIL_CHIP_RESET
#if CONFIG_USE_LOG_MONITOR && DMA_LOG
		LOG_INFOR(TAG, "Chip will reset after %ds.", CONFIG_WAIT_FOR_RESET_TIME);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
		systick_delay_ms(CONFIG_WAIT_FOR_RESET_TIME*1000U);
		__NVIC_SystemReset();
#endif /* CONFIG_FAIL_CHIP_RESET */
		return ret;
	}

	__NVIC_SetPriority(_IRQn, _conf -> interruptpriority);

	_state = STM_READY;

	return ret;
}

void dma::register_event_handler(void (*Event_Callback)(void *Parameter, dma_event_t event), void *Parameter){
	this -> Event_Callback = Event_Callback;
	this -> Parameter = Parameter;
}

stm_ret_t dma::start(uint32_t Src_Address, uint32_t Dest_Address, uint32_t Number_Data){
	stm_ret_t ret;

	if(_state == STM_READY){
		_state = STM_BUSY;
		_conf -> stream -> CR &=~ (DMA_SxCR_EN);
		ret = wait_flag_in_register_timeout(&(_conf -> stream -> CR), DMA_SxCR_EN, FLAG_RESET, 50U);
		if(is_timeout(&ret)){
#if CONFIG_USE_LOG_MONITOR && DMA_LOG
			LOG_ERROR(TAG, "%s -> %s -> Wait flag timeout.", __FILE__, __FUNCTION__);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
			set_return_line(&ret, __LINE__);
			return ret;
		}

		_conf -> stream -> CR &=~ DMA_SxCR_DBM;
		_conf -> stream -> NDTR = Number_Data;
		if(_conf -> direction == DMA_MEM_TO_PERIPH){
			_conf -> stream -> PAR = Dest_Address;
			_conf -> stream -> M0AR = Src_Address;
		}
		else if(_conf -> direction == DMA_PERIH_TO_MEM){
			_conf -> stream -> PAR = Src_Address;
			_conf -> stream -> M0AR = Dest_Address;
		}
		ClearAllIntrFlag();

		_conf -> stream -> CR  |= _conf -> interruptselect | DMA_SxCR_TEIE | DMA_SxCR_DMEIE;
		_conf -> stream -> CR |= DMA_SxCR_EN;

		__NVIC_ClearPendingIRQ(_IRQn);
		__NVIC_EnableIRQ(_IRQn);

#if CONFIG_USE_LOG_MONITOR && DMA_LOG
		LOG_ERROR(TAG, "%s -> %s -> DMA running.", __FILE__, __FUNCTION__);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
	}
	else{
#if CONFIG_USE_LOG_MONITOR && DMA_LOG
		LOG_ERROR(TAG, "%s -> %s -> DMA state busy, can't start.", __FILE__, __FUNCTION__);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
		set_return(&ret, STM_BUSY, __LINE__);
		return ret;
	}

	return ret;
}

stm_ret_t dma::stop(void){
	stm_ret_t ret;

	if(_state == STM_BUSY){
		_state = STM_READY;
		_conf -> stream -> CR  &= ~(DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | DMA_SxCR_HTIE);
		_conf -> stream -> FCR &=~ DMA_SxFCR_FEIE;
		_conf -> stream -> CR  &=~ DMA_SxCR_EN;

		ret = wait_flag_in_register_timeout(&(_conf -> stream -> CR), DMA_SxCR_EN, FLAG_RESET, 5U);
		if(is_timeout(&ret)){
#if CONFIG_USE_LOG_MONITOR && DMA_LOG
			LOG_ERROR(TAG, "%s -> %s -> Wait flag timeout.", __FILE__, __FUNCTION__);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
			set_return_line(&ret, __LINE__);
			return ret;
		}

		ClearAllIntrFlag();

		__NVIC_ClearPendingIRQ(_IRQn);
		__NVIC_DisableIRQ(_IRQn);

#if CONFIG_USE_LOG_MONITOR && DMA_LOG
		LOG_ERROR(TAG, "%s -> %s -> DMA stoped.", __FILE__, __FUNCTION__);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
	}
	else{
#if CONFIG_USE_LOG_MONITOR && DMA_LOG
		LOG_ERROR(TAG, "%s -> %s -> DMA state ready, can't stop.", __FILE__, __FUNCTION__);
#endif /* CONFIG_USE_LOG_MONITOR && DMA_LOG */
		set_return(&ret, STM_ERR, __LINE__);
		return ret;
	}

	return ret;
}

stm_ret_t dma::poll_for_tranfer(dma_interruptselect_t PollLevel, uint32_t TimeOut){
	stm_ret_t ret;
	__IO uint32_t PollValue = 0U, tick, isr;

	if(_state != STM_BUSY){
		set_return(&ret, STM_BUSY, __LINE__);
		return ret;
	}

	if(_conf -> stream -> CR & DMA_SxCR_CIRC){
		set_return(&ret, STM_UNSUPPORTED, __LINE__);
		return ret;
	}

	if(PollLevel == DMA_TRANSFER_COMPLETE_INTERRUPT){
		PollValue = (uint32_t)(0x20U << _Intr_Index);
	}
	else if(PollLevel == DMA_HALF_TRANSFER_INTERRUPT){
		PollValue = (uint32_t)(0x20U << _Intr_Index);
	}
	else{
		set_return(&ret, STM_ERR, __LINE__);

		return ret;
	}

	tick = get_tick();
	isr = GetISR();
	while(!(isr & PollValue)){
		if(TimeOut != NO_TIMEOUT){
			if(get_tick() - tick > TimeOut){
				set_return(&ret, STM_TIMEOUT, __LINE__);
				return ret;
			}
		}
		// CHECK TRANFER ERROR.
		isr = GetISR();
		if(isr & (0x01U << _Intr_Index)){ // FEIE.
			ClearIFCR(0x01U << _Intr_Index);
			break;
		}
		if(isr & (0x04U << _Intr_Index)){ // DMEIE.
			ClearIFCR(0x04U << _Intr_Index);
			break;
		}
		if(isr & (0x08U << _Intr_Index)){ // TEIE.
			ClearIFCR(0x08U << _Intr_Index);
			break;
		}
	}

	if(isr & (0x08U << _Intr_Index)){
		stop();
		ClearIFCR((DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0) << _Intr_Index);
		set_return(&ret, STM_ERR, __LINE__);
		return ret;
	}
	if(PollLevel == DMA_TRANSFER_COMPLETE_INTERRUPT){
		ClearIFCR((DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0) << _Intr_Index);
		_state = STM_READY;
	}
	else{
		ClearIFCR(DMA_LIFCR_CHTIF0 << _Intr_Index);
	}

	return ret;
}

uint16_t dma::get_counter(void){
	return _conf -> stream -> NDTR;
}

dma_config_t *dma::get_config(void){
	return _conf;
}


void DMA_IRQ_Handler(DMA_TypeDef *pdma, DMA_Stream_TypeDef *stream, dma *dmaptr){
	uint8_t num_stream = (((uint32_t)stream & 0xFFU) - 16U) / 24U;
	uint8_t index = Channel_Index[num_stream];
	dma_event_t event = DMA_EVENT_NOEVENT;

	if((num_stream < 4)? (pdma -> LISR & (DMA_LISR_HTIF0 << index)) : (pdma -> HISR & (DMA_HISR_HTIF4 << index))){
		if(stream -> CR & DMA_SxCR_HTIE){
			(num_stream < 4)? (pdma -> LIFCR = (DMA_LIFCR_CHTIF0 << index)) : (pdma -> HIFCR = (DMA_HIFCR_CHTIF4 << index));
			if(!(stream -> CR & DMA_SxCR_CIRC)){
				stream -> CR &=~ DMA_SxCR_HTIE;
				event = DMA_EVENT_HALF_TRANFER;
				goto EventCB;
			}
		}
	}

	if((num_stream < 4)? (pdma -> LISR & (DMA_LISR_TCIF0 << index)) : (pdma -> HISR & (DMA_HISR_TCIF4 << index))){
		stream -> CR &=~ (DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | DMA_SxCR_HTIE);
		stream -> FCR &=~ (DMA_SxFCR_FEIE);
		(num_stream < 4)? (pdma -> LIFCR = (0x3FU << index)) : (pdma -> HIFCR = (0x3FU << index));
		if(!(stream -> CR & DMA_SxCR_CIRC)){
			stream -> CR &=~ DMA_SxCR_TCIE;
			event = DMA_EVENT_TRANFER_COMPLETE;
			goto EventCB;
		}
	}

	if((num_stream < 4)? (pdma -> LISR & (DMA_LISR_TEIF0 << index)) : (pdma -> HISR & (DMA_HISR_TEIF4 << index))){
		stream -> CR &=~ DMA_SxCR_TEIE;
		(num_stream < 4)? (pdma -> LIFCR = (DMA_LIFCR_CTEIF0 << index)) : (pdma -> HIFCR = (DMA_HIFCR_CTEIF4 << index));
		event = DMA_EVENT_TRANFER_ERROR;
		goto EventCB;
	}

	EventCB:
	if(dmaptr -> Event_Callback != NULL) dmaptr -> Event_Callback(dmaptr -> Parameter, event);
}

/**
 *  DMA1 IRQ HANDLER
 */
#if defined(DMA1_Stream0)
dma dma1_0(DMA1);
dma_t dma1_stream0 = &dma1_0;
void DMA1_Stream0_IRQHandler(void);
void DMA1_Stream0_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream0, &dma1_0);
}
#endif /* defined(DMA1_Stream0) */
#if defined(DMA1_Stream1)
dma dma1_1(DMA1);
dma_t dma1_stream1 = &dma1_1;
void DMA1_Stream1_IRQHandler(void);
void DMA1_Stream1_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream1, &dma1_1);
}
#endif /* defined(DMA1_Stream1) */
#if defined(DMA1_Stream2)
dma dma1_2(DMA1);
dma_t dma1_stream2 = &dma1_2;
void DMA1_Stream2_IRQHandler(void);
void DMA1_Stream2_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream2, &dma1_2);
}
#endif /* defined(DMA1_Stream2) */
#if defined(DMA1_Stream3)
dma dma1_3(DMA1);
dma_t dma1_stream3 = &dma1_3;
void DMA1_Stream3_IRQHandler(void);
void DMA1_Stream3_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream3, &dma1_3);
}
#endif /* defined(DMA1_Stream3) */
#if defined(DMA1_Stream4)
dma dma1_4(DMA1);
dma_t dma1_stream4 = &dma1_4;
void DMA1_Stream4_IRQHandler(void);
void DMA1_Stream4_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream4, &dma1_4);

}
#endif /* defined(DMA1_Stream4) */
#if defined(DMA1_Stream5)
dma dma1_5(DMA1);
dma_t dma1_stream5 = &dma1_5;
void DMA1_Stream5_IRQHandler(void);
void DMA1_Stream5_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream5, &dma1_5);
}
#endif /* defined(DMA1_Stream5) */
#if defined(DMA1_Stream6)
dma dma1_6(DMA1);
dma_t dma1_stream6 = &dma1_6;
void DMA1_Stream6_IRQHandler(void);
void DMA1_Stream6_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream6, &dma1_6);
}
#endif /* defined(DMA1_Stream6) */
#if defined(DMA1_Stream7)
dma dma1_7(DMA1);
dma_t dma1_stream7 = &dma1_7;
void DMA1_Stream7_IRQHandler(void);
void DMA1_Stream7_IRQHandler(void){
	DMA_IRQ_Handler(DMA1, DMA1_Stream7, &dma1_7);
}
#endif /* defined(DMA1_Stream7) */



/**
 *  DMA2 IRQ HANDLER
 */
#if defined(DMA2_Stream0)
dma dma2_0(DMA2);
dma_t dma2_stream0 = &dma2_0;
void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream0, &dma2_0);
}
#endif /* defined(DMA2_Stream0) */
#if defined(DMA2_Stream1)
dma dma2_1(DMA2);
dma_t dma2_stream1 = &dma2_1;
void DMA2_Stream1_IRQHandler(void);
void DMA2_Stream1_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream1, &dma2_1);
}
#endif /* defined(DMA2_Stream1) */
#if defined(DMA2_Stream2)
dma dma2_2(DMA2);
dma_t dma2_stream2 = &dma2_2;
void DMA2_Stream2_IRQHandler(void);
void DMA2_Stream2_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream2, &dma2_2);
}
#endif /* defined(DMA2_Stream2) */
#if defined(DMA2_Stream3)
dma dma2_3(DMA2);
dma_t dma2_stream3 = &dma2_3;
void DMA2_Stream3_IRQHandler(void);
void DMA2_Stream3_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream3, &dma2_3);
}
#endif /* defined(DMA2_Stream3) */
#if defined(DMA2_Stream4)
dma dma2_4(DMA2);
dma_t dma2_stream4 = &dma2_4;
void DMA2_Stream4_IRQHandler(void);
void DMA2_Stream4_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream4, &dma2_4);
}
#endif /* defined(DMA2_Stream4) */
#if defined(DMA2_Stream5)
dma dma2_5(DMA2);
dma_t dma2_stream5 = &dma2_5;
void DMA2_Stream5_IRQHandler(void);
void DMA2_Stream5_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream5, &dma2_5);
}
#endif /* defined(DMA2_Stream5) */
#if defined(DMA2_Stream6)
dma dma2_6(DMA2);
dma_t dma2_stream6 = &dma2_6;
void DMA2_Stream6_IRQHandler(void);
void DMA2_Stream6_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream6, &dma2_6);
}
#endif /* defined(DMA2_Stream6) */
#if defined(DMA2_Stream7)
dma dma2_7(DMA2);
dma_t dma2_stream7 = &dma2_7;
void DMA2_Stream7_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void){
	DMA_IRQ_Handler(DMA2, DMA2_Stream7, &dma2_7);
}
#endif /* defined(DMA2_Stream7) */


#endif /* ENABLE_DMA */


