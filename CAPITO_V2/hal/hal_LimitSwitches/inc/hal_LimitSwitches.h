/*
 * hal_LimitSwitches.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_LIMITSWITCHES_INC_HAL_LIMITSWITCHES_H_
#define HAL_HAL_LIMITSWITCHES_INC_HAL_LIMITSWITCHES_H_
/*******************************************************************************
*                               includes                             *
*******************************************************************************/



/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
#define LIMIT_SWITCHES_PORT                 0
#define LIMIT_SWITCHES_PIN_DN               10
#define LIMIT_SWITCHES_PIN_UP               11
//#define LIMIT_SWITCHES_PIN_DN               4
//#define LIMIT_SWITCHES_PIN_UP               5

/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/


/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/

void HAL_LimitSwitches_PINS_Init(void);
void HAL_LimitSwitches_PINS_Write(uint8_t ui8PinNumber, Bool On);
Bool HAL_LimitSwitches_PINS_Read(uint8_t ui8PinNumber);


#endif /* HAL_HAL_LIMITSWITCHES_INC_HAL_LIMITSWITCHES_H_ */
