/*
 * hal_ExtraUart.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_EXTRAUART_HAL_EXTRAUART_H_
#define HAL_HAL_EXTRAUART_HAL_EXTRAUART_H_
/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
#define EXTRA_UART_UART2_PORT                 2
#define EXTRA_UART_TX2_PIN                    8
#define EXTRA_UART_RX2_PIN                    9
#define UART2_BUFFER_SIZE                     7


/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/


/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void HAL_ExtraUart_UART2Pins_Configure(void);
void HAL_ExtraUart_UART2_Init(void);
void HAL_ExtraUart_UART2_Send(LPC_USART_T *pUART, const void *data, int numBytes);
void HAL_ExtraUart_UART2_Read(LPC_USART_T *pUART, void *data, int numBytes);
void HAL_ExtraUart_UART2_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes);
void HAL_ExtraUart_UART2_ReadBlocking(LPC_USART_T *pUART, void *data, int numBytes);




#endif /* HAL_HAL_EXTRAUART_HAL_EXTRAUART_H_ */
