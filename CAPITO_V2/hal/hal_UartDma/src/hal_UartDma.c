/*
 * hal_UartDma.c
 *
 *  Created on: Jan 28, 2024
 *      Author: Rahma
 */
#include "lpc_types.h"
#include "board.h"
#include "LPC17xx.h"
#include "uart_17xx_40xx.h"
#include "gpdma_17xx_40xx.h"
#include "hal_UartDma.h"



// Buffer for UART1 DMA transfer
//uint8_t uart1DmaBuffer[UART1_DMA_SIZE];
//GPDMA_CH_CFG_T dmaCfg;

/* DMA variables and functions declaration */
static uint8_t dmaChannelNumTx, dmaChannelNumRx;

static volatile uint32_t channelTC;	/* Terminal Counter flag for Channel */
static volatile uint32_t channelTCErr;
static FunctionalState  isDMATx = DISABLE;
uint8_t receiveBuffer[50]={0};
uint8_t ui8READREG=0;
volatile uint8_t g_RxINTStatus = 0;
uint32_t g_intsrc=0;
uint8_t g_ui8Count2=0;
static uint8_t uartDMA_menu[4]= {0X01,0x02,0x03,0x04};



//void DMA_IRQHandler(void)
//{
//	uint8_t dmaChannelNum;
//	if (isDMATx) {
//		dmaChannelNum = dmaChannelNumTx;
//	}
//	else
//	{
//		dmaChannelNum = dmaChannelNumRx;
//	}
//	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum) == SUCCESS)
//	{
//		channelTC++;
//	}
//	else
//	{
//
//		channelTCErr++;
//	}
//}

//void UART3_IRQHandler(void)
//{
//	if (Chip_UART_GetIntsEnabled(LPC_UART3) & UART_IER_RBRINT)
//	{
//		g_intsrc = Chip_UART_ReadIntIDReg(LPC_UART3);
//		g_intsrc = (g_intsrc & 0x8);
//		// UART receive buffer full interrupt
//		while ((Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR))
//		{
//			//RDA
//			if(g_intsrc == 0)
//			{
//
//				receiveBuffer[g_ui8Count2] = Chip_UART_ReadByte(LPC_UART3);
//				g_ui8Count2++;
//			}
//			//CTI
//			if (g_intsrc == 8)
//			{
//				receiveBuffer[g_ui8Count2] = Chip_UART_ReadByte(LPC_UART3);
//				g_ui8Count2++;
//			}
//
//			break;
//		}
//	}
//}
//void UART3_IRQHandler(void)
//{
//	uint32_t ui32IIR = Chip_UART_ReadIntIDReg(LPC_UART3);
//	uint32_t ui32LSR ;
//
//
//	ui32IIR >>=1;
//	ui32IIR &= 0x7;
//	/* Check the interrupt ID Register  */
//	if (ui32IIR == (0x3)) // RLS
//	{
//		ui32LSR = Chip_UART_ReadLineStatus(LPC_UART3);
//
//		// check if error occurs
//		if (ui32LSR & (UART_LSR_OE|UART_LSR_PE|UART_LSR_FE|UART_LSR_RXFE|UART_LSR_BI))
//		{
//			Chip_UART_ReadByte(LPC_UART3);
//		}
//	}
//
//	else if (ui32IIR == (0x2 )) // RDA
//	{
//		while (Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR)
//		{
//			receiveBuffer[g_ui8Count2]= Chip_UART_ReadByte(LPC_UART3);
//			g_ui8Count2++;;
//		}
//	}
//	else if (ui32IIR == (0x6)) // CTI
//	{
//		while (Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR)
//		{
//			receiveBuffer[g_ui8Count2] = Chip_UART_ReadByte(LPC_UART3);
//			g_ui8Count2++;;
//		}
//	}
//}

//void UART1_IRQHandler(void)
//{
//	uint32_t ui32IIR ;
//	//	uint32_t ui32LSR ;
//	//	if (Chip_UART_GetIntsEnabled(LPC_UART1) & UART_IER_THREINT)
//	//	{
//	//		Tx1Complete = TRUE;
//	//		//Chip_UART_IntDisable(LPC_UART3, UART_IER_THREINT);
//	//	}
//	if (Chip_UART_GetIntsEnabled(LPC_UART1) & UART_IER_RBRINT)
//	{
//		ui32IIR = Chip_UART_ReadIntIDReg(LPC_UART1);
//		ui32IIR = (ui32IIR & 0x8);
//		// UART receive buffer full interrupt
//		while ((Chip_UART_ReadLineStatus(LPC_UART1) & UART_LSR_RDR))
//		{
//			//RDA
//			if(ui32IIR == 0)
//			{
//
//				receiveBuffer[g_ui8Count2] = Chip_UART_ReadByte(LPC_UART1);
//				g_ui8Count2++;
//			}
//			//CTI
//			if (ui32IIR == 8)
//			{
//				receiveBuffer[g_ui8Count2] = Chip_UART_ReadByte(LPC_UART1);
//				g_ui8Count2++;
//			}
//
//			break;
//		}
//	}
//}

static void HAL_DMA_Init(void)
{
	//	uart3
	Chip_IOCON_PinMuxSet(LPC_IOCON, DMA_UART3_PORT, DMA_RX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
	// Configure UART1 TX pin (P2.0)
	Chip_IOCON_PinMuxSet(LPC_IOCON, DMA_UART3_PORT, DMA_TX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);

	//	uart1
	//	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 1, IOCON_MODE_PULLUP |IOCON_FUNC2);
	//
	//	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 0, IOCON_MODE_PULLUP |IOCON_FUNC2);
	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);
	/* Setting GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);
}
static void HAL_DMA_DeInit(void)
{
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumTx);
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumRx);
	NVIC_DisableIRQ(DMA_IRQn);
}

/* DMA routine for example_uart */
void HAL_DMA_Test(void)
{
	Board_UART_Init(LPC_UART3);
	Chip_UART_Init(LPC_UART3);
	Chip_UART_SetBaud(LPC_UART3, 115200);
	Chip_UART_ConfigData(LPC_UART3, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_TXEnable(LPC_UART3);
	Chip_UART_SetupFIFOS(LPC_UART3, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS));
	Chip_UART_SetupFIFOS(LPC_UART3,  UART_FCR_FIFO_EN | UART_FCR_DMAMODE_SEL );
	Chip_UART_SetupFIFOS(LPC_UART3, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV3 );
	NVIC_EnableIRQ(UART3_IRQn);
	Chip_UART_IntEnable(LPC_UART3, UART_IER_THREINT);
	Chip_UART_IntEnable(LPC_UART3, UART_IER_RBRINT);

	//	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 1, IOCON_MODE_PULLUP |IOCON_FUNC2);
	//	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 0, IOCON_MODE_PULLUP |IOCON_FUNC2);
	//	Board_UART_Init(LPC_UART1);
	//	Chip_UART_Init(LPC_UART1);
	//	Chip_UART_SetBaud(LPC_UART1, 115200);
	//	Chip_UART_ConfigData(LPC_UART1, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	//	Chip_UART_TXEnable(LPC_UART1);
	//	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS));
	//	Chip_UART_SetupFIFOS(LPC_UART1,  UART_FCR_FIFO_EN | UART_FCR_DMAMODE_SEL );
	//	Chip_UART_SetupFIFOS(LPC_UART1, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV3 );
	//	NVIC_EnableIRQ(UART1_IRQn);
	//	Chip_UART_IntEnable(LPC_UART1, UART_IER_THREINT);
	//	Chip_UART_IntEnable(LPC_UART1, UART_IER_RBRINT);

	//	 Enable UART interrupts for transmit and receive
	NVIC_EnableIRQ(UART3_IRQn);

	HAL_DMA_Init();
	NVIC_EnableIRQ(DMA_IRQn);

	dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_UART1_Tx);
	isDMATx = ENABLE;
	channelTC = channelTCErr = 0;
	//	NVIC_EnableIRQ(DMA_IRQn);
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx, (uint32_t) &uartDMA_menu, GPDMA_CONN_UART3_Tx, GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA ,sizeof(uartDMA_menu));
	while (!channelTC) {}

	//	dmaChannelNumRx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_UART3_Rx);
	//	isDMATx = DISABLE;
	//	channelTC = channelTCErr = 0;
	//	NVIC_EnableIRQ(DMA_IRQn);
	//	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumRx, GPDMA_CONN_UART3_Rx,(uint32_t)receiveBuffer, GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,8);
	//	while (!channelTC) {}

	dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_UART1_Tx);
	isDMATx = ENABLE;
	channelTC = channelTCErr = 0;
	//	NVIC_EnableIRQ(DMA_IRQn);
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx,(uint32_t) receiveBuffer,GPDMA_CONN_UART3_Tx, GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA, sizeof(receiveBuffer));
	while (!channelTC) {}
	//	HAL_DMA_DeInit();
}


