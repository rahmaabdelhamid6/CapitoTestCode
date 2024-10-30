/*
 * hal_Modem.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_MODEM_INC_HAL_MODEM_H_
#define HAL_HAL_MODEM_INC_HAL_MODEM_H_

/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
#define MODEM_UART1_PORT                 2
#define MODEM_TX1_PIN                    0
#define MODEM_RX1_PIN                    1
#define UART1_BUFFER_SIZE                     7

/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/
extern volatile Bool Tx1Complete;
extern volatile  Bool Rx1Complete;

/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void HAL_Modem_UART1Pins_Configure(void);
void HAL_Modem_UART1_Init(void);
void HAL_Modem_UART1_Send(LPC_USART_T *pUART, const void *data, int numBytes);
void HAL_Modem_UART1_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes);
void HAL_Modem_UART1_ReadBlocking(LPC_USART_T *pUART,  void *data, int numBytes);
void HAL_Modem_UART1_Read(LPC_USART_T *pUART,  void *data, int numBytes);


#endif /* HAL_HAL_MODEM_INC_HAL_MODEM_H_ */
