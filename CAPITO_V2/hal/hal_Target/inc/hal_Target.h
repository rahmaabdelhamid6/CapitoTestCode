/*
 * hal_Target.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_TARGET_INC_HAL_TARGET_H_
#define HAL_HAL_TARGET_INC_HAL_TARGET_H_

/*******************************************************************************
*                               includes                             *
*******************************************************************************/
#define TARGET_PORT                 2
#define TARGET_PIN_MV_DN            4
#define TARGET_PIN_MV_UP            5

/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/


/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/


/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void HAL_Target_PINS_Init(void);
void HAL_Target_PINS_Write(uint8_t ui8PinNumber, Bool On);
Bool HAL_Target_PINS_Read(uint8_t ui8PinNumber);

#endif /* HAL_HAL_TARGET_INC_HAL_TARGET_H_ */
