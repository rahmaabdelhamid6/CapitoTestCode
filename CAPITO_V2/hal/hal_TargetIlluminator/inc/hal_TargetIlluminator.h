/*
 * hal_TargetIlluminator.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_TARGETILLUMINATOR_INC_HAL_TARGETILLUMINATOR_H_
#define HAL_HAL_TARGETILLUMINATOR_INC_HAL_TARGETILLUMINATOR_H_

/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
#define TARGET_ILLUMINATOR_PORT                 2
#define TARGET_ILLUMINATOR_PIN                  2

/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/


/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void HAL_TargetIlluminator_PIN_Init(void);
void HAL_TargetIlluminator_PIN_Write(uint8_t ui8PinNumber, Bool On);
Bool HAL_TargetIlluminator_PIN_Read(uint8_t ui8PinNumber);

#endif /* HAL_HAL_TARGETILLUMINATOR_INC_HAL_TARGETILLUMINATOR_H_ */
