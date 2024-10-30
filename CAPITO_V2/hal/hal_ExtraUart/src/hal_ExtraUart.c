/*
 * hal_ExtraUart.c
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
#include "hal_ExtraUart.h"

/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/
uint8_t g_aui8Tx2Buffer[UART2_BUFFER_SIZE] = {1, 2, 3, 4, 5,6,7};
uint8_t g_aui8Rx2Buffer[UART2_BUFFER_SIZE] = {0};

volatile Bool Tx2Complete = FALSE;
volatile Bool Rx2Complete = FALSE;

/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void UART2_IRQHandler(void)
{
	if (Chip_UART_GetIntsEnabled(LPC_UART2) & UART_IER_THREINT)
	{
		Tx2Complete = TRUE;
		Chip_UART_IntDisable(LPC_UART2, UART_IER_THREINT);
	}

	if (Chip_UART_GetIntsEnabled(LPC_UART2) & UART_IER_RBRINT) {
		// UART receive buffer full interrupt
		while ((Chip_UART_ReadLineStatus(LPC_UART2) & UART_LSR_RDR))
		{
			Rx2Complete = TRUE;  // All bytes received
			Chip_UART_IntDisable(LPC_UART1, UART_IER_RBRINT);
			break;
		}
	}
}


void HAL_ExtraUart_UART2Pins_Configure(void)
{
	// Configure UART2 RX pin (P2.9)
	Chip_IOCON_PinMux(LPC_IOCON, EXTRA_UART_UART2_PORT, EXTRA_UART_RX2_PIN, IOCON_MODE_PULLUP, IOCON_FUNC2);

	// Configure UART2 TX pin (P2.8)
	Chip_IOCON_PinMux(LPC_IOCON, EXTRA_UART_UART2_PORT, EXTRA_UART_TX2_PIN, IOCON_MODE_PULLUP, IOCON_FUNC2);
}


void HAL_ExtraUart_UART2_Init(void)
{
	HAL_ExtraUart_UART2Pins_Configure();
	Chip_UART_Init(LPC_UART2);
	Chip_UART_SetBaud(LPC_UART2, 115200);
	Chip_UART_ConfigData(LPC_UART2, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_TXEnable(LPC_UART2);
	// Enable UART interrupts for transmit and receive
//	Chip_UART_IntEnable(LPC_UART2, UART_IER_THREINT | UART_IER_RBRINT);
//	NVIC_EnableIRQ(UART2_IRQn);
}

void HAL_ExtraUart_UART2_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes)
{
	Chip_UART_SendBlocking(pUART, data, numBytes);
}
void HAL_ExtraUart_UART2_ReadBlocking(LPC_USART_T *pUART,  void *data, int numBytes)
{
	Chip_UART_ReadBlocking(pUART, data, numBytes);
}

void HAL_ExtraUart_UART2_Send(LPC_USART_T *pUART, const void *data, int numBytes)
{
	Chip_UART_Send(pUART, data, numBytes);
}
void HAL_ExtraUart_UART2_Read(LPC_USART_T *pUART,  void *data, int numBytes)
{
	Chip_UART_Read(pUART, data, numBytes);
}
