/*
 * hal_Debug.c
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */


/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "lpc_types.h"
#include "board.h"
#include "LPC17xx.h"
#include "hal_Debug.h"

/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/
volatile Bool txComplete = FALSE;
volatile Bool rxComplete = FALSE;
uint32_t intsrc=0;

uint8_t g_aui8RxBuffer2[50];
uint8_t g_ui8Count = 0;

/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
//void UART3_IRQHandler(void)
//{
//	uint32_t ui32IIR ;
//	uint32_t ui32LSR ;
//	if (Chip_UART_GetIntsEnabled(LPC_UART3) & UART_IER_THREINT)
//	{
//		txComplete = TRUE;
//		//		Chip_UART_IntDisable(LPC_UART3, UART_IER_THREINT);
//	}
//	if (Chip_UART_GetIntsEnabled(LPC_UART3) & UART_IER_RBRINT)
//		{
//		ui32IIR = Chip_UART_ReadIntIDReg(LPC_UART3);
//		ui32IIR = (ui32IIR & 0x8);
//			// UART receive buffer full interrupt
//			while ((Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR))
//			{
//				//RDA
//				if(ui32IIR == 0)
//				{
//
//					g_aui8RxBuffer2[g_ui8Count] = Chip_UART_ReadByte(LPC_UART3);
//					g_ui8Count++;
//				}
//				//CTI
//				if (ui32IIR == 8)
//				{
//					g_aui8RxBuffer2[g_ui8Count] = Chip_UART_ReadByte(LPC_UART3);
//					g_ui8Count++;
//				}
//
//				break;
//			}
//		}
//
//}


//void UART3_IRQHandler(void)
//{
//
//	if (Chip_UART_GetIntsEnabled(LPC_UART3) & UART_IER_THREINT)
//	{
//		txComplete = TRUE;
//		Chip_UART_IntDisable(LPC_UART3, UART_IER_THREINT);
//	}
//
//	if (Chip_UART_GetIntsEnabled(LPC_UART3) & UART_IER_RBRINT)
//	{
//		intsrc = Chip_UART_ReadIntIDReg(LPC_UART3);
//		intsrc = (intsrc & 0x8);
//		// UART receive buffer full interrupt
//		while ((Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR))
//		{
//			//			if(intsrc & UART_IIR_INTID_CTI)
//			//RDA
//			if(intsrc == 0)
//			{
//				rxComplete = TRUE;
//				Chip_UART_Read(LPC_UART3, g_aui8RxBuffer2, 14);  //lhad el trigger level
//				// Clear the interrupt pending
////				Chip_UART_IntDisable(LPC_UART3, UART_IER_RBRINT);
//
//			}
//			//else if (intsrc & UART_IIR_INTID_RDA)
//			//CTI
//			if (intsrc == 8)
//			{
//				rxComplete = TRUE;
//				Chip_UART_Read(LPC_UART3, &g_aui8RxBuffer2[14], 6);  //f halet ni 3arfa n elly gay 20 lw mch 3arfa el size hahtag a while loop w b start index mn b3d el trigrer level
//				// Clear the interrupt pending
////				Chip_UART_IntDisable(LPC_UART3, UART_IER_RBRINT);
//			}
//
//			break;
//		}
//	}
//}

void HAL_Debug_UART3Pins_Configure(void)
{
	// Configure UART3 RX pin (P0.1)
	//	Chip_IOCON_PinMux(LPC_IOCON, DEBUG_UART3_PORT, DEBUG_RX3_PIN, IOCON_MODE_PULLUP, IOCON_FUNC2);
	Chip_IOCON_PinMuxSet(LPC_IOCON, DEBUG_UART3_PORT, DEBUG_RX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
	// Configure UART3 TX pin (P0.0)
	//	Chip_IOCON_PinMux(LPC_IOCON, DEBUG_UART3_PORT, DEBUG_TX3_PIN, IOCON_MODE_PULLUP, IOCON_FUNC2);
	Chip_IOCON_PinMuxSet(LPC_IOCON, DEBUG_UART3_PORT, DEBUG_TX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
}

void HAL_Debug_UART3_Init(void)
{
	HAL_Debug_UART3Pins_Configure();
	Chip_UART_Init(LPC_UART3);
	Chip_UART_SetBaud(LPC_UART3, 115200);
	Chip_UART_ConfigData(LPC_UART3, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_TXEnable(LPC_UART3);
	Chip_UART_SetupFIFOS(LPC_UART3, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2 );

	//	//	 Enable UART interrupts for transmit and receive
	//	Chip_UART_IntEnable(LPC_UART3, UART_IER_THREINT | UART_IER_RBRINT);
	//		NVIC_EnableIRQ(UART3_IRQn);
}
void HAL_Debug_UART3_Send(LPC_USART_T *pUART, const void *data, int numBytes)
{
	Chip_UART_Send(pUART, data, numBytes);
}

void HAL_Debug_UART3_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes)
{
	Chip_UART_SendBlocking(pUART, data, numBytes);
}


void HAL_Debug_UART3_Read(LPC_USART_T *pUART,  void *data, int numBytes)
{
	Chip_UART_Read(pUART, data, numBytes);
}

int  HAL_Debug_UART3_ReadBlocking(LPC_USART_T *pUART,  void *data, int numBytes)
{
	Chip_UART_ReadBlocking(pUART, data, numBytes);
	return Chip_UART_ReadBlocking(pUART, data, numBytes);
}
///////////////////////////////////////////////////////////////////////////////////
