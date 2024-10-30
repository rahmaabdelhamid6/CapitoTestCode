/*
 * hal_ModemDMA.h
 *
 *  Created on: Feb 21, 2024
 *      Author: Rahma
 */

#ifndef HAL_HAL_MODEMDMA_INC_HAL_MODEMDMA_H_
#define HAL_HAL_MODEMDMA_INC_HAL_MODEMDMA_H_


/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/

#define MODEM_DMA_UART3_PORT                 0
#define MODEM_DMA_TX3_PIN                    0
#define MODEM_UART3_RX3_PIN                  1

#define	MODEM_DMA_MAX_MSG_SIZE			255 /* Bytes */
#define	MODEM_RX_MAX_MSG_SIZE			32 //Trigger level 1 .. RDA count =16
#define	MODEM_TX_MAX_MSG_SIZE			32
#define	MODEM_SYNQ_BYTE                 0x55
/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/

typedef enum
{
	MODEM_DMA_STATUS_SUCCESS,
	MODEM_DMA_STATUS_BUFFER_EMPTY,
	MODEM_DMA_STATUS_BUFFER_HAS_DATA,
	MODEM_DMA_STATUS_BUFFER_HAS_COMPLETE,
	MODEM_DMA_STATUS_BUFFER_FULL

}ModemDmaStatus_et;

typedef enum
{
	MODEM_DMA_RX_UPDATE = 1,
	MODEM_DMA_TX_UPDATE
}ModemDmaTxRxUpdate_et;

/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/
extern uint8_t dmaChannelNumTx;
extern uint8_t uartDMA_menu[10];
/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void HAL_ModemDMA_UART1Pins_Configure(void);
ModemDmaStatus_et HAL_ModemDMA_Init(uint8_t ui8TrigerLevel);
ModemDmaStatus_et HAL_ModemDMA_setTxData(uint8_t *aui8TxData,uint8_t ui8TxSize);
uint8_t HAL_ModemDMA_getRxDataCount(void);
ModemDmaStatus_et HAL_ModemDMA_getRxData(uint8_t aui8RxData[],uint8_t ui8RxSize);
ModemDmaStatus_et HAL_ModemDMA_update(ModemDmaTxRxUpdate_et etTxRxUpdate);
void HAL_ModemDma_Test(void);

#endif /* HAL_HAL_MODEMDMA_INC_HAL_MODEMDMA_H_ */
