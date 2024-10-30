/*
 * hal_ModemDMA.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Rahma
 */

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/

#include "lpc_types.h"
#include "board.h"
#include "LPC17xx.h"
#include "string.h"
#include "uart_17xx_40xx.h"
#include "gpdma_17xx_40xx.h"
#include "hal_ModemDMA.h"


/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/
// Private Defines
#define		MODEM_RX_BUFFER_MAX_MSGS_NUM			10
#define 	MODEM_DMA_TX_BUFFER_MAX_MSGS_NUM	    8

#define MODEM_DMA_CHANNEL_IDLE_YES	          0U
#define MODEM_DMA_CHANNEL_IDLE_NO		      1U


typedef struct
{
	uint8_t aui8Data[MODEM_TX_MAX_MSG_SIZE];
	uint8_t ui8DataSize;

}DataBuffer_st;

typedef struct
{
	DataBuffer_st astDataBuffer[MODEM_DMA_TX_BUFFER_MAX_MSGS_NUM];
	uint8_t ui8WriteIndex;
	uint8_t ui8ReadIndex;
	ModemDmaStatus_et etStatus;
}ModemDmaTxBuffer_st;


typedef struct
{
	uint8_t ui8ReadIndex;
	uint8_t ui8DataSize;
	uint8_t ui8count;
	uint8_t aui8Data[255];
	ModemDmaStatus_et etStatus;
}ModemDmaRxBuffer_st;


ModemDmaTxBuffer_st g_stModemDmaTxBuffer;
ModemDmaRxBuffer_st g_stModemDmaRxBuffer;

uint8_t dmaChannelNumTx=0;
uint8_t g_aui8ModemRxBuffer[MODEM_DMA_MAX_MSG_SIZE];
uint8_t g_ui8RxCount=0;

volatile uint8_t g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_YES;
uint8_t receiveBuffer2[256]={0};
uint8_t uartDMA_menu[10]= {0X01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A};
uint8_t uartDMA_menu2[10]= {0X05,0x02,0x08,0x04,0x05,0x09,0x07,0x08,0x09,0x0A};
uint8_t g_ui8RxSize=0;
int bytesToWrite=0;
uint8_t ui8RDAcount=0;
uint8_t ui8CountAfterSynq=0;
uint8_t g_ui8SynqByte=0;
/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void DMA_IRQHandler(void)
{
//	g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_NO;
	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNumTx) == SUCCESS)
	{
		//		g_stModemDmaTxBuffer.ui8count=0;
		g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_YES;
	}
}
void UART3_IRQHandler(void)
{
	uint32_t ui32IIR = Chip_UART_ReadIntIDReg(LPC_UART3);
	uint32_t ui32LSR ;

	ui32IIR >>=1;
	ui32IIR &= 0x7;
	/* Check the interrupt ID Register  */
	if (ui32IIR == (0x3)) // RLS
	{
		ui32LSR = Chip_UART_ReadLineStatus(LPC_UART3);

		// check if error occurs
		if (ui32LSR & (UART_LSR_OE|UART_LSR_PE|UART_LSR_FE|UART_LSR_RXFE|UART_LSR_BI))
		{
			Chip_UART_ReadByte(LPC_UART3);
		}
	}

	else if (ui32IIR == (0x2 )) // RDA
	{
		while (Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR)
		{
			g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8count]= Chip_UART_ReadByte(LPC_UART3);
			g_stModemDmaRxBuffer.ui8count++;
			ui8RDAcount++;
		}
	}
	else if (ui32IIR == (0x6)) // CTI
	{
		while (Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR)
		{
			g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8count] = Chip_UART_ReadByte(LPC_UART3);
			g_stModemDmaRxBuffer.ui8count++;
			ui8RDAcount++;
		}
	}
	HAL_ModemDMA_update(MODEM_DMA_RX_UPDATE);
}
void HAL_ModemDMA_UART1Pins_Configure(void)
{
	// Configure UART3 RX pin (P2.1)
	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_UART3_RX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);

	// Configure UART1 TX pin (P2.0)
	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_DMA_TX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
}
static void HAL_DMA_Init(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_UART3_RX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
	// Configure UART1 TX pin (P2.0)
	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_DMA_TX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);
	/* Setting GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);
}
static void HAL_UART_Init(uint8_t ui8TrigerLevel)
{
	Board_UART_Init(LPC_UART3);
	Chip_UART_Init(LPC_UART3);
	Chip_UART_SetBaud(LPC_UART3, 115200);
	Chip_UART_ConfigData(LPC_UART3, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_TXEnable(LPC_UART3);

	Chip_UART_SetupFIFOS(LPC_UART3, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS));
	Chip_UART_SetupFIFOS(LPC_UART3,  UART_FCR_FIFO_EN | UART_FCR_DMAMODE_SEL );
	Chip_UART_SetupFIFOS(LPC_UART3, UART_FCR_FIFO_EN | ui8TrigerLevel );
	NVIC_EnableIRQ(UART3_IRQn);
	//	Chip_UART_IntEnable(LPC_UART3, UART_IER_THREINT);
	Chip_UART_IntEnable(LPC_UART3, UART_IER_RBRINT);
}
ModemDmaStatus_et HAL_ModemDMA_Init(uint8_t ui8TrigerLevel)
{
	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;
	HAL_UART_Init(ui8TrigerLevel);
	HAL_ModemDMA_UART1Pins_Configure();
	HAL_DMA_Init();

	// TODO: Zeroing g_ui32ModemRxBufferWriteIndex
	memset((void *)&g_stModemDmaRxBuffer,0,  sizeof(ModemDmaRxBuffer_st));

	// TODO: Zeroing g_ui322ModemTxBufferWriteIndex
	memset((void *)&g_stModemDmaTxBuffer,0,  sizeof(ModemDmaTxBuffer_st));

	g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;

	g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
	g_stModemDmaTxBuffer.ui8WriteIndex = 0 ;
//	g_stModemDmaTxBuffer.ui8count = 0 ;
	g_stModemDmaRxBuffer.ui8DataSize = 0 ;
	g_stModemDmaRxBuffer.ui8ReadIndex = 0 ;
	g_stModemDmaRxBuffer.ui8count = 0;

	return etModemDmaStatus;
}
static void HAL_ModemDMA_DeInit(void)
{
	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumTx);
	NVIC_DisableIRQ(DMA_IRQn);

	g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_NO;
	// TODO: Zeroing g_ui32GPSRxBufferWriteIndex
	memset((void *)&g_stModemDmaRxBuffer,0,  sizeof(ModemDmaRxBuffer_st));

	// TODO: Zeroing g_ui32GPSRxBufferWriteIndex
	memset((void *)&g_stModemDmaTxBuffer,0,  sizeof(ModemDmaTxBuffer_st));

	g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;

	g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
}
ModemDmaStatus_et HAL_ModemDMA_setTxData(uint8_t *aui8TxData,uint8_t ui8TxSize)
{
	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;
	bytesToWrite=ui8TxSize;

	switch(g_stModemDmaTxBuffer.etStatus)
	{
	case MODEM_DMA_STATUS_BUFFER_EMPTY:
	case MODEM_DMA_STATUS_BUFFER_HAS_DATA:
		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
		//		if (g_stModemDmaTxBuffer.ui8count + bytesToWrite > MODEM_DMA_MAX_MSG_SIZE)
		//		{
		//			bytesToWrite = MODEM_DMA_MAX_MSG_SIZE - g_stModemDmaTxBuffer.ui8count;
		//		}
		//		g_stModemDmaTxBuffer.ui8count += bytesToWrite;
		memcpy((void *)g_stModemDmaTxBuffer.astDataBuffer[g_stModemDmaTxBuffer.ui8WriteIndex].aui8Data, (void *) aui8TxData , bytesToWrite );
		g_stModemDmaTxBuffer.astDataBuffer[g_stModemDmaTxBuffer.ui8WriteIndex].ui8DataSize = bytesToWrite;
		g_stModemDmaTxBuffer.ui8WriteIndex = (g_stModemDmaTxBuffer.ui8WriteIndex + 1) % MODEM_DMA_TX_BUFFER_MAX_MSGS_NUM;
		g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
		break;

	case MODEM_DMA_STATUS_BUFFER_FULL:
		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_FULL;
		//		g_stModemDmaTxBuffer.ui8count=0;
		//		if (bytesToWrite <= ui8TxSize)
		//		{
		//			dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_UART3_Tx);
		//			memcpy((void *)&g_stModemDmaTxBuffer.aui8Data[g_stModemDmaTxBuffer.ui8WriteIndex], &aui8TxData[bytesToWrite], (ui8TxSize - bytesToWrite));
		//			g_stModemDmaTxBuffer.ui8WriteIndex= (g_stModemDmaTxBuffer.ui8WriteIndex + ui8TxSize - bytesToWrite) % MODEM_DMA_MAX_MSG_SIZE;
		//			g_stModemDmaTxBuffer.ui8count += ui8TxSize - bytesToWrite;
		//			//			g_stModemDmaTxBuffer.ui8count += ui8TxSize - bytesToWrite;
		//		}
		break;
	default:
		break;
	}
	return etModemDmaStatus;
}
uint8_t HAL_ModemDMA_getRxDataCount(void)
{
	return 	g_stModemDmaRxBuffer.ui8DataSize;
}
ModemDmaStatus_et HAL_ModemDMA_getRxData(uint8_t aui8RxData[],uint8_t ui8RxSize)
{
	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;
	switch(g_stModemDmaRxBuffer.etStatus)
	{
	case MODEM_DMA_STATUS_BUFFER_EMPTY:
		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
		g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
		//lw galo goz' mn el data bs fadl rda count b rakm bsbb no el data elly gat  tani byte feh elly hwa haykhudha 3la nha length mknsh sah w kan a'al mn el bytes elly mb3ota f hincrement el read index w asfr rda count 3chan lw gat data gdida
		if(g_stModemDmaRxBuffer.ui8ReadIndex != g_stModemDmaRxBuffer.ui8count && ui8CountAfterSynq == 0)  // != + add more check on ui8CountAfterSynq ==0
		{
			g_stModemDmaRxBuffer.ui8ReadIndex = (g_stModemDmaRxBuffer.ui8ReadIndex + ui8RDAcount) % MODEM_DMA_MAX_MSG_SIZE;
			ui8RDAcount=0;
		}
		break;
	case MODEM_DMA_STATUS_BUFFER_HAS_DATA:
		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
		//ui8RDAcount++; //mkano hna aw f el interrupt f halet lw trigger level 0
		g_ui8SynqByte=g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8ReadIndex];
		if (g_ui8SynqByte == MODEM_SYNQ_BYTE )
		{
			ui8CountAfterSynq++;
		}
		if (ui8RDAcount == g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8ReadIndex+1] ) //wesl ll length
		{
			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_COMPLETE;
			g_stModemDmaRxBuffer.ui8DataSize = ui8RDAcount;
		}
		else
		{
			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
			g_stModemDmaRxBuffer.ui8DataSize += ui8RDAcount;
		}
		break;
	case MODEM_DMA_STATUS_BUFFER_HAS_COMPLETE:
		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_HAS_COMPLETE;
		if (ui8RDAcount == ui8CountAfterSynq)
		{
			memset((void *)aui8RxData,0, 32);
			memcpy((void *)aui8RxData,(void *) &g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8ReadIndex],ui8RDAcount );
			g_stModemDmaRxBuffer.ui8ReadIndex = (g_stModemDmaRxBuffer.ui8ReadIndex + ui8RDAcount) %MODEM_DMA_MAX_MSG_SIZE;
			g_stModemDmaRxBuffer.ui8DataSize = ui8RDAcount;
			g_ui8SynqByte=0;
			ui8RDAcount=0;
			ui8CountAfterSynq = 0;
			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
		}
		break;
	case MODEM_DMA_STATUS_BUFFER_FULL:
		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_FULL;
		g_stModemDmaRxBuffer.ui8count= 	0;
		memset (g_stModemDmaRxBuffer.aui8Data ,0,MODEM_DMA_MAX_MSG_SIZE);
		g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
		ui8RDAcount=0;
		break;
	default:
		break;
	}
	return 	etModemDmaStatus;

}
ModemDmaStatus_et HAL_ModemDMA_update(ModemDmaTxRxUpdate_et etTxRxUpdate )
{
	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;

	if (etTxRxUpdate == MODEM_DMA_RX_UPDATE)
	{
		/* Update Rx Buffer Status */
		if (g_stModemDmaRxBuffer.ui8count == 0)
		{
			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
		}
		else if (g_stModemDmaRxBuffer.ui8count == MODEM_DMA_MAX_MSG_SIZE)
		{
			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_FULL;
		}
		else
		{
			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
		}
	}

	else if (etTxRxUpdate == MODEM_DMA_TX_UPDATE)
	{
		/* Start Sending Data To GPs If available */
		if (g_stModemDmaTxBuffer.etStatus == MODEM_DMA_STATUS_BUFFER_HAS_DATA)
		{
			// Check If GPS Tx Channel Is IDLE
			if (g_ui8ModemDmaTxChannelStatus == MODEM_DMA_CHANNEL_IDLE_YES)
			{
				g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_NO;
				NVIC_EnableIRQ(DMA_IRQn);
				Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx, (uint32_t)g_stModemDmaTxBuffer.astDataBuffer[g_stModemDmaTxBuffer.ui8ReadIndex].aui8Data,GPDMA_CONN_UART3_Tx, GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,g_stModemDmaTxBuffer.astDataBuffer[g_stModemDmaTxBuffer.ui8ReadIndex].ui8DataSize);
				g_stModemDmaTxBuffer.ui8ReadIndex = (g_stModemDmaTxBuffer.ui8ReadIndex +1)%MODEM_DMA_TX_BUFFER_MAX_MSGS_NUM;
			} // end of Check If GPS Tx Channel Is IDLE
		}
		/* Update Tx Buffer Status */
		if (g_stModemDmaTxBuffer.ui8ReadIndex == g_stModemDmaTxBuffer.ui8WriteIndex)
		{
			g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
		}
		else if (((g_stModemDmaTxBuffer.ui8WriteIndex + 1) % MODEM_DMA_TX_BUFFER_MAX_MSGS_NUM ) == g_stModemDmaTxBuffer.ui8ReadIndex)
		{
			g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_FULL;
		}
		else
		{
			g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
		}
	}
	return 	etModemDmaStatus;
}
void HAL_ModemDma_Test()
{
	dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_UART3_Tx);
	HAL_ModemDMA_setTxData(uartDMA_menu,sizeof(uartDMA_menu));
	HAL_ModemDMA_setTxData(uartDMA_menu2,sizeof(uartDMA_menu));
	HAL_ModemDMA_update(MODEM_DMA_TX_UPDATE);
}



///*
// * hal_ModemDMA.c
// *
// *  Created on: Feb 21, 2024
// *      Author: Rahma
// */
//
///*******************************************************************************
// *                                Includes                                     *
// *******************************************************************************/
//
//#include "lpc_types.h"
//#include "board.h"
//#include "LPC17xx.h"
//#include "string.h"
//#include "uart_17xx_40xx.h"
//#include "gpdma_17xx_40xx.h"
//#include "hal_ModemDMA.h"
//
//
///*******************************************************************************
// *                               Global Variables                             *
// *******************************************************************************/
//// Private Defines
//#define		MODEM_RX_BUFFER_MAX_MSGS_NUM			10
//#define 	MODEM_DMA_TX_BUFFER_MAX_MSGS_NUM	    10
//
//#define MODEM_DMA_CHANNEL_IDLE_YES	          0U
//#define MODEM_DMA_CHANNEL_IDLE_NO		      1U
//
//typedef struct
//{
//	uint8_t ui8WriteIndex;
//	uint8_t ui8DataSize;
//	uint8_t ui8count;
//	uint8_t aui8Data[255];
//	ModemDmaStatus_et etStatus;
//}ModemDmaTxBuffer_st;
//
//
//typedef struct
//{
//	uint8_t ui8ReadIndex;
//	uint8_t ui8DataSize;
//	uint8_t ui8count;
//	uint8_t aui8Data[255];
//	ModemDmaStatus_et etStatus;
//}ModemDmaRxBuffer_st;
//
//
//ModemDmaTxBuffer_st g_stModemDmaTxBuffer;
//ModemDmaRxBuffer_st g_stModemDmaRxBuffer;
//
//uint8_t dmaChannelNumTx=0;
//uint8_t g_aui8ModemRxBuffer[MODEM_DMA_MAX_MSG_SIZE];
//uint8_t g_ui8RxCount=0;
//
//volatile uint8_t g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_YES;
//uint8_t receiveBuffer2[256]={0};
//uint8_t uartDMA_menu[10]= {0X01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A};
//uint8_t g_ui8RxSize=0;
//int bytesToWrite=0;
//uint8_t ui8RDAcount=0;
//uint8_t ui8CountAfterSynq=0;
//uint8_t g_ui8SynqByte=0;
///*******************************************************************************
// *                              Function Definition                            *
// *******************************************************************************/
//void DMA_IRQHandler(void)
//{
//	g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_NO;
//	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNumTx) == SUCCESS)
//	{
//		//		g_stModemDmaTxBuffer.ui8count=0;
//		g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_YES;
//	}
//}
//void UART3_IRQHandler(void)
//{
//	uint32_t ui32IIR = Chip_UART_ReadIntIDReg(LPC_UART3);
//	uint32_t ui32LSR ;
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
//			g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8count]= Chip_UART_ReadByte(LPC_UART3);
//			g_stModemDmaRxBuffer.ui8count++;
//			ui8RDAcount++;
//		}
//	}
//	else if (ui32IIR == (0x6)) // CTI
//	{
//		while (Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_RDR)
//		{
//			g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8count] = Chip_UART_ReadByte(LPC_UART3);
//			g_stModemDmaRxBuffer.ui8count++;
//			ui8RDAcount++;
//		}
//	}
//	HAL_ModemDMA_update(MODEM_DMA_RX_UPDATE);
//}
//void HAL_ModemDMA_UART1Pins_Configure(void)
//{
//	// Configure UART3 RX pin (P2.1)
//	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_UART3_RX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
//
//	// Configure UART1 TX pin (P2.0)
//	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_DMA_TX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
//}
//static void HAL_DMA_Init(void)
//{
//	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_UART3_RX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
//	// Configure UART1 TX pin (P2.0)
//	Chip_IOCON_PinMuxSet(LPC_IOCON, MODEM_DMA_UART3_PORT, MODEM_DMA_TX3_PIN, IOCON_MODE_PULLUP |IOCON_FUNC2);
//	/* Initialize GPDMA controller */
//	Chip_GPDMA_Init(LPC_GPDMA);
//	/* Setting GPDMA interrupt */
//	NVIC_DisableIRQ(DMA_IRQn);
//	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
//	NVIC_EnableIRQ(DMA_IRQn);
//}
//static void HAL_UART_Init(uint8_t ui8TrigerLevel)
//{
//	Board_UART_Init(LPC_UART3);
//	Chip_UART_Init(LPC_UART3);
//	Chip_UART_SetBaud(LPC_UART3, 115200);
//	Chip_UART_ConfigData(LPC_UART3, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
//	Chip_UART_TXEnable(LPC_UART3);
//
//	Chip_UART_SetupFIFOS(LPC_UART3, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS));
//	Chip_UART_SetupFIFOS(LPC_UART3,  UART_FCR_FIFO_EN | UART_FCR_DMAMODE_SEL );
//	Chip_UART_SetupFIFOS(LPC_UART3, UART_FCR_FIFO_EN | ui8TrigerLevel );
//	NVIC_EnableIRQ(UART3_IRQn);
//	//	Chip_UART_IntEnable(LPC_UART3, UART_IER_THREINT);
//	Chip_UART_IntEnable(LPC_UART3, UART_IER_RBRINT);
//}
//ModemDmaStatus_et HAL_ModemDMA_Init(uint8_t ui8TrigerLevel)
//{
//	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;
//	HAL_UART_Init(ui8TrigerLevel);
//	HAL_ModemDMA_UART1Pins_Configure();
//	HAL_DMA_Init();
//
//	// TODO: Zeroing g_ui32ModemRxBufferWriteIndex
//	memset((void *)&g_stModemDmaRxBuffer,0,  sizeof(ModemDmaRxBuffer_st));
//
//	// TODO: Zeroing g_ui322ModemTxBufferWriteIndex
//	memset((void *)&g_stModemDmaTxBuffer,0,  sizeof(ModemDmaTxBuffer_st));
//
//	g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//
//	g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//	g_stModemDmaTxBuffer.ui8WriteIndex = 0 ;
//	g_stModemDmaTxBuffer.ui8count = 0 ;
//	g_stModemDmaRxBuffer.ui8DataSize = 0 ;
//	g_stModemDmaRxBuffer.ui8ReadIndex = 0 ;
//	g_stModemDmaRxBuffer.ui8count = 0;
//
//	return etModemDmaStatus;
//}
//static void HAL_ModemDMA_DeInit(void)
//{
//	Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNumTx);
//	NVIC_DisableIRQ(DMA_IRQn);
//
//	g_ui8ModemDmaTxChannelStatus = MODEM_DMA_CHANNEL_IDLE_NO;
//	// TODO: Zeroing g_ui32GPSRxBufferWriteIndex
//	memset((void *)&g_stModemDmaRxBuffer,0,  sizeof(ModemDmaRxBuffer_st));
//
//	// TODO: Zeroing g_ui32GPSRxBufferWriteIndex
//	memset((void *)&g_stModemDmaTxBuffer,0,  sizeof(ModemDmaTxBuffer_st));
//
//	g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//
//	g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//}
//ModemDmaStatus_et HAL_ModemDMA_setTxData(uint8_t *aui8TxData,uint8_t ui8TxSize)
//{
//	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;
//	bytesToWrite=ui8TxSize;
//
//	switch(g_stModemDmaTxBuffer.etStatus)
//	{
//	case MODEM_DMA_STATUS_BUFFER_EMPTY:
//	case MODEM_DMA_STATUS_BUFFER_HAS_DATA:
//		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
//		if (g_stModemDmaTxBuffer.ui8WriteIndex + bytesToWrite > MODEM_DMA_MAX_MSG_SIZE)
//		{
//			bytesToWrite = MODEM_DMA_MAX_MSG_SIZE - g_stModemDmaTxBuffer.ui8WriteIndex;
//		}
//		memcpy((void *)&g_stModemDmaTxBuffer.aui8Data[g_stModemDmaTxBuffer.ui8WriteIndex], (void *) aui8TxData , bytesToWrite );
//		g_stModemDmaTxBuffer.ui8WriteIndex = (g_stModemDmaTxBuffer.ui8WriteIndex + bytesToWrite) % MODEM_DMA_MAX_MSG_SIZE;
//		g_stModemDmaTxBuffer.ui8count += bytesToWrite;
//		g_stModemDmaTxBuffer.ui8DataSize = bytesToWrite;
//		g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
//
//		break;
//
//	case MODEM_DMA_STATUS_BUFFER_FULL:
//		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_FULL;
//		//		g_stModemDmaTxBuffer.ui8count=0;
//		//		if (bytesToWrite <= ui8TxSize)
//		//		{
//		//			dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_UART3_Tx);
//		//			memcpy((void *)&g_stModemDmaTxBuffer.aui8Data[g_stModemDmaTxBuffer.ui8WriteIndex], &aui8TxData[bytesToWrite], (ui8TxSize - bytesToWrite));
//		//			g_stModemDmaTxBuffer.ui8WriteIndex= (g_stModemDmaTxBuffer.ui8WriteIndex + ui8TxSize - bytesToWrite) % MODEM_DMA_MAX_MSG_SIZE;
//		//			g_stModemDmaTxBuffer.ui8count += ui8TxSize - bytesToWrite;
//		//			//			g_stModemDmaTxBuffer.ui8count += ui8TxSize - bytesToWrite;
//		//		}
//		break;
//	default:
//		break;
//	}
//	return etModemDmaStatus;
//}
//uint8_t HAL_ModemDMA_getRxDataCount(void)
//{
//	return 	g_stModemDmaRxBuffer.ui8DataSize;
//}
//ModemDmaStatus_et HAL_ModemDMA_getRxData(uint8_t aui8RxData[],uint8_t ui8RxSize)
//{
//	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;
//	switch(g_stModemDmaRxBuffer.etStatus)
//	{
//	case MODEM_DMA_STATUS_BUFFER_EMPTY:
//		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//		g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//		//lw galo goz' mn el data bs fadl rda count b rakm bsbb no el data elly gat  tani byte feh elly hwa haykhudha 3la nha length mknsh sah w kan a'al mn el bytes elly mb3ota f hincrement el read index w asfr rda count 3chan lw gat data gdida
//		if(g_stModemDmaRxBuffer.ui8ReadIndex != g_stModemDmaRxBuffer.ui8count && ui8CountAfterSynq == 0)  // != + add more check on ui8CountAfterSynq ==0
//		{
//			g_stModemDmaRxBuffer.ui8ReadIndex = (g_stModemDmaRxBuffer.ui8ReadIndex + ui8RDAcount) % MODEM_DMA_MAX_MSG_SIZE;
//			ui8RDAcount=0;
//		}
//		break;
//	case MODEM_DMA_STATUS_BUFFER_HAS_DATA:
//		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
//		//ui8RDAcount++; //mkano hna aw f el interrupt f halet lw trigger level 0
//		g_ui8SynqByte=g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8ReadIndex];
//		if (g_ui8SynqByte == MODEM_SYNQ_BYTE )
//		{
//			ui8CountAfterSynq++;
//		}
//		if (ui8RDAcount == g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8ReadIndex+1] ) //wesl ll length
//		{
//			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_COMPLETE;
//			g_stModemDmaRxBuffer.ui8DataSize = ui8RDAcount;
//		}
//		else
//		{
//			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//			g_stModemDmaRxBuffer.ui8DataSize += ui8RDAcount;
//		}
//		break;
//	case MODEM_DMA_STATUS_BUFFER_HAS_COMPLETE:
//		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_HAS_COMPLETE;
//		if (ui8RDAcount == ui8CountAfterSynq)
//		{
//			memset((void *)aui8RxData,0, 32);
//			memcpy((void *)aui8RxData,(void *) &g_stModemDmaRxBuffer.aui8Data[g_stModemDmaRxBuffer.ui8ReadIndex],ui8RDAcount );
//			g_stModemDmaRxBuffer.ui8ReadIndex = (g_stModemDmaRxBuffer.ui8ReadIndex + ui8RDAcount) %MODEM_DMA_MAX_MSG_SIZE;
//			g_stModemDmaRxBuffer.ui8DataSize = ui8RDAcount;
//			g_ui8SynqByte=0;
//			ui8RDAcount=0;
//			ui8CountAfterSynq = 0;
//			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//		}
//		break;
//	case MODEM_DMA_STATUS_BUFFER_FULL:
//		etModemDmaStatus = MODEM_DMA_STATUS_BUFFER_FULL;
//		g_stModemDmaRxBuffer.ui8count= 	0;
//		memset (g_stModemDmaRxBuffer.aui8Data ,0,MODEM_DMA_MAX_MSG_SIZE);
//		g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//		ui8RDAcount=0;
//		break;
//	default:
//		break;
//	}
//	return 	etModemDmaStatus;
//
//}
//ModemDmaStatus_et HAL_ModemDMA_update(ModemDmaTxRxUpdate_et etTxRxUpdate )
//{
//	ModemDmaStatus_et etModemDmaStatus = MODEM_DMA_STATUS_SUCCESS;
//
//	if (etTxRxUpdate == MODEM_DMA_RX_UPDATE)
//	{
//		/* Update Rx Buffer Status */
//		if (g_stModemDmaRxBuffer.ui8count == 0)
//		{
//			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//		}
//		else if (g_stModemDmaRxBuffer.ui8count == MODEM_DMA_MAX_MSG_SIZE)
//		{
//			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_FULL;
//		}
//		else
//		{
//			g_stModemDmaRxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
//		}
//	}
//
//	else if (etTxRxUpdate == MODEM_DMA_TX_UPDATE)
//	{
//		/* Start Sending Data To GPs If available */
//		if (g_stModemDmaTxBuffer.etStatus == MODEM_DMA_STATUS_BUFFER_HAS_DATA)
//		{
//			// Check If GPS Tx Channel Is IDLE
//			if (g_ui8ModemDmaTxChannelStatus == MODEM_DMA_CHANNEL_IDLE_YES)
//			{
//				NVIC_EnableIRQ(DMA_IRQn);
//				Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNumTx, (uint32_t)g_stModemDmaTxBuffer.aui8Data,GPDMA_CONN_UART3_Tx, GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,g_stModemDmaTxBuffer.ui8DataSize);
//			} // end of Check If GPS Tx Channel Is IDLE
//		}
//		/* Update Tx Buffer Status */
//		if (g_stModemDmaTxBuffer.ui8count == 0)
//		{
//			g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_EMPTY;
//		}
//		else if (g_stModemDmaTxBuffer.ui8count == MODEM_DMA_MAX_MSG_SIZE)
//		{
//			g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_FULL;
//		}
//		else
//		{
//			g_stModemDmaTxBuffer.etStatus = MODEM_DMA_STATUS_BUFFER_HAS_DATA;
//		}
//	}
//	return 	etModemDmaStatus;
//}
//void HAL_ModemDma_Test()
//{
//	dmaChannelNumTx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_UART3_Tx);
//	HAL_ModemDMA_setTxData(uartDMA_menu,sizeof(uartDMA_menu));
//	HAL_ModemDMA_update(MODEM_DMA_TX_UPDATE);
//}
