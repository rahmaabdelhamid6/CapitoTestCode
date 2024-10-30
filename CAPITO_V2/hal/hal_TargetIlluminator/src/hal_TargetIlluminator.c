/*
 * hal_TargetIlluminator.c
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */


/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "lpc_types.h"
#include "board.h"
#include "LPC17xx.h"
#include "hal_Targetilluminator.h"

/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/


/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void HAL_TargetIlluminator_PIN_Init(void)
{
	/* configure Pin PIO2_2 as GPIO pin */

	Chip_IOCON_PinMux(LPC_IOCON,TARGET_ILLUMINATOR_PORT, TARGET_ILLUMINATOR_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	/* Set the PIO_2 as OUTPUT */
	Chip_GPIO_WriteDirBit(LPC_GPIO, TARGET_ILLUMINATOR_PORT, TARGET_ILLUMINATOR_PIN, true);
}


/* Sets the state of a board Pin to on or off */
void HAL_TargetIlluminator_PIN_Write(uint8_t ui8PinNumber, Bool On)
{
	Chip_GPIO_WritePortBit(LPC_GPIO, TARGET_ILLUMINATOR_PORT, ui8PinNumber, On);
}


/* Returns the current state of a board Pin */
Bool HAL_TargetIlluminator_PIN_Read(uint8_t ui8PinNumber)
{
	Bool state = false;

	state = Chip_GPIO_ReadPortBit(LPC_GPIO, TARGET_ILLUMINATOR_PORT, ui8PinNumber);

	return state;
}
