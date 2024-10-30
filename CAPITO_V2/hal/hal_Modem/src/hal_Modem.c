/*
 * hal_Modem.c
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */


/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "lpc_types.h"
#include "board.h"
#include "hal_Modem.h"

/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/
uint8_t g_aui8Tx1Buffer[UART1_BUFFER_SIZE] = {1, 2, 3, 4, 5,6,7};
uint8_t g_aui8Rx1Buffer[50] = {0};

volatile Bool Tx1Complete = FALSE;
volatile Bool Rx1Complete = FALSE;
uint8_t g_ui8Count3;


/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void UART1_IRQHandler(void)
{
	uint32_t ui32IIR ;
	uint32_t ui32LSR ;
	if (Chip_UART_GetIntsEnabled(LPC_UART1) & UART_IER_THREINT)
	{
		Tx1Complete = TRUE;
		//Chip_UART_IntDisable(LPC_UART3, UART_IER_THREINT);
	}
	if (Chip_UART_GetIntsEnabled(LPC_UART1) & UART_IER_RBRINT)
	{
		ui32IIR = Chip_UART_ReadIntIDReg(LPC_UART1);
		ui32IIR = (ui32IIR & 0x8);
		// UART receive buffer full interrupt
		while ((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR))
		{
			//RDA
			if(ui32IIR == 0)
			{

				g_aui8Rx1Buffer[g_ui8Count3] = Chip_UART_ReadByte(LPC_UART1);
				g_ui8Count3++;
			}
			//CTI
			if (ui32IIR == 8)
			{
				g_aui8Rx1Buffer[g_ui8Count3] = Chip_UART_ReadByte(LPC_UART1);
				g_ui8Count3++;
			}

			break;
		}
	}
}

//void UART1_IRQHandler(void)
//{
//
//	uint32_t ui32IIR ;
//	uint32_t ui32LSR ;
//	if (Chip_UART_GetIntsEnabled(LPC_UART1) & UART_IER_THREINT)
//	{
//		Tx1Complete = TRUE;
//		//		Chip_UART_IntDisable(LPC_UART1, UART_IER_THREINT);
//	}
//
//	//	if (Chip_UART_GetIntsEnabled(LPC_UART1) & UART_IER_RBRINT) {
//	//		// UART receive buffer full interrupt
//	//		while ((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR))
//	//		{
//	//			Rx1Complete = TRUE;  // All bytes received
//	//			Chip_UART_IntDisable(LPC_UART1, UART_IER_RBRINT);
//	//			break;
//	//		}
//	//	}
//
//	ui32IIR = Chip_UART_ReadIntIDReg(LPC_UART1);
//	ui32IIR >>=1;
//	ui32IIR &= 0x7;
//	/* Check the interrupt ID Register  */
//	if (ui32IIR == (0x3)) // RLS
//	{
//		ui32LSR = Chip_UART_ReadLineStatus(LPC_UART1);
//
//		// check if error occurs
//		if (ui32LSR & (UART_LSR_OE|UART_LSR_PE|UART_LSR_FE|UART_LSR_RXFE|UART_LSR_BI))
//		{
//			Chip_UART_ReadByte(LPC_UART1);
//		}
//	}
//
//	else if (ui32IIR == (0x2 )) // RDA
//	{
//		while (Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR)
//		{
//
//			g_aui8Rx1Buffer[g_ui8Count3] = Chip_UART_ReadByte(LPC_UART1);
//			g_ui8Count3++;
//		}
//	}
//	else if (ui32IIR == (0x6)) // CTI
//	{
//		while (Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR)
//		{
//			g_aui8Rx1Buffer[g_ui8Count3] = Chip_UART_ReadByte(LPC_UART1);
//			g_ui8Count3++;
//		}
//	}
//
//}



void HAL_Modem_UART1Pins_Configure(void)
{
	// Configure UART3 RX pin (P2.1)
	//	Chip_IOCON_PinMux(LPC_IOCON, MODEM_UART1_PORT, MODEM_RX1_PIN, IOCON_MODE_PULLUP, IOCON_FUNC2);
	//	// Configure UART1 TX pin (P2.0)
	//	Chip_IOCON_PinMux(LPC_IOCON, MODEM_UART1_PORT, MODEM_TX1_PIN, IOCON_MODE_PULLUP, IOCON_FUNC2);

	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_UART1_PORT, MODEM_RX1_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);

	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_UART1_PORT, MODEM_TX1_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);

}

void HAL_Modem_UART1_Init(void)
{
	HAL_Modem_UART1Pins_Configure();
	Chip_UART_Init(LPC_UART1);
	Chip_UART_SetBaud(LPC_UART1, 115200);
	Chip_UART_ConfigData(LPC_UART1, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_TXEnable(LPC_UART1);
	// Enable UART interrupts for transmit and receive
	//	Chip_UART_IntEnable(LPC_UART1, UART_IER_THREINT | UART_IER_RBRINT);
	//	NVIC_EnableIRQ(UART1_IRQn);

}
void HAL_Modem_UART1_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes)
{
	Chip_UART_SendBlocking(pUART, data, numBytes);
}
void HAL_Modem_UART1_ReadBlocking(LPC_USART_T *pUART,  void *data, int numBytes)
{
	Chip_UART_ReadBlocking(pUART, data, numBytes);
}
void HAL_Modem_UART1_Send(LPC_USART_T *pUART, const void *data, int numBytes)
{
	Chip_UART_Send(pUART, data, numBytes);
}
void HAL_Modem_UART1_Read(LPC_USART_T *pUART,  void *data, int numBytes)
{
	Chip_UART_Read(pUART, data, numBytes);
}
