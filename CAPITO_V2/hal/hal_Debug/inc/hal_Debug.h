/*
 * hal_Debug.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_DEBUG_INC_HAL_DEBUG_H_
#define HAL_HAL_DEBUG_INC_HAL_DEBUG_H_

/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/


#define DEBUG_UART3_PORT                 0
#define DEBUG_TX3_PIN                    0
#define DEBUG_RX3_PIN                    1

#define UART_RXBUFFER_SIZE     50
#define UART_BUFFER_SIZE       14
/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/
extern volatile Bool txComplete;
extern volatile Bool rxComplete;
extern uint8_t rxBuffer[UART_RXBUFFER_SIZE];

/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/

void HAL_Debug_UART3Pins_Configure(void);
void HAL_Debug_UART3_Init(void);
void HAL_Debug_UART3_Send(LPC_USART_T *pUART, const void *data, int numBytes);
void HAL_Debug_UART3_Read(LPC_USART_T *pUART,  void *data, int numBytes);
void HAL_Debug_UART3_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes);
int HAL_Debug_UART3_ReadBlocking(LPC_USART_T *pUART,  void *data, int numBytes);

#endif /* HAL_HAL_DEBUG_INC_HAL_DEBUG_H_ */
