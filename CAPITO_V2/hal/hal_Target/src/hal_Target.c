/*
 * hal_Target.c
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
#include "hal_Target.h"
/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/


/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void HAL_Target_PINS_Init(void)
{
	/* configure Pin PIO2_4 , Pin PIO2_5 as GPIO pin */

	//DOWN
	Chip_IOCON_PinMux(LPC_IOCON,TARGET_PORT, TARGET_PIN_MV_DN, IOCON_MODE_INACT, IOCON_FUNC0);

	/* Set the PIO_4 as OUTPUT */
//		Chip_GPIO_WriteDirBit(LPC_GPIO, TARGET_PORT, TARGET_PIN_MV_DN, true);
		Chip_GPIO_SetPinDIR(LPC_GPIO, TARGET_PORT, TARGET_PIN_MV_DN, true);

	//UP
	Chip_IOCON_PinMux(LPC_IOCON,TARGET_PORT, TARGET_PIN_MV_UP, IOCON_MODE_INACT, IOCON_FUNC0);
	/* Set the PIO_5 as OUTPUT */
//	Chip_GPIO_WriteDirBit(LPC_GPIO, TARGET_PORT, TARGET_PIN_MV_UP, true);
	Chip_GPIO_SetPinDIR(LPC_GPIO, TARGET_PORT, TARGET_PIN_MV_UP, true);
}


/* Sets the state of a board Pin to on or off */
void HAL_Target_PINS_Write(uint8_t ui8PinNumber, Bool On)
{

	if (ui8PinNumber == TARGET_PIN_MV_DN)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, TARGET_PORT, ui8PinNumber, On);
	}

	if (ui8PinNumber == TARGET_PIN_MV_UP)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, TARGET_PORT, ui8PinNumber, On);
	}
}


/* Returns the current state of a board Pin */
Bool HAL_Target_PINS_Read(uint8_t ui8PinNumber)
{
	Bool state = false;

	if (ui8PinNumber == TARGET_PIN_MV_DN) {
		state = Chip_GPIO_ReadPortBit(LPC_GPIO, TARGET_PORT, ui8PinNumber);
	}

	if (ui8PinNumber == TARGET_PIN_MV_UP) {
		state = Chip_GPIO_ReadPortBit(LPC_GPIO, TARGET_PORT, ui8PinNumber);
	}

	return state;
}
