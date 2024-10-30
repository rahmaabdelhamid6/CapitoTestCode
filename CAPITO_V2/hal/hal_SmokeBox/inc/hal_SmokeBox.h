/*
 * hal_SmokeBox.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_SMOKEBOX_INC_HAL_SMOKEBOX_H_
#define HAL_HAL_SMOKEBOX_INC_HAL_SMOKEBOX_H_


/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
//
//#define GPIO_PORT0_NUM                      0
//#define GPIO_BIT2_NUM                       2
//#define GPIO_BIT3_NUM                       3

#define SMOKE_BOX_PORT                        0
#define SMOKE_BOX_12V_PIN_PULSE                2
#define SMOKE_BOX_5V_PIN_PULSE                 3


/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/


/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void HAL_SmokeBox_PINS_Init(void);
void HAL_SmokeBox_PINS_Write(uint8_t ui8PinNumber, Bool On);
Bool HAL_SmokeBox_PINS_Read(uint8_t ui8PinNumber);


#endif /* HAL_HAL_SMOKEBOX_INC_HAL_SMOKEBOX_H_ */
