/*
 * hal_LimitSwitches.c
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
#include "hal_LimitSwitches.h"

/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/


/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void HAL_LimitSwitches_PINS_Init(void)
{
	/* configure Pin PIO0_10 , Pin PIO0_10 as GPIO pin */

	//DOWN
//	Chip_IOCON_PinMux(LPC_IOCON,LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_DN, IOCON_MODE_PULLDOWN, IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_DN , IOCON_MODE_PULLDOWN |IOCON_FUNC0);

	/* Set the PIO_10 as input */
//	Chip_GPIO_WriteDirBit(LPC_GPIO, LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_DN, false);

	Chip_GPIO_WriteDirBit(LPC_GPIO, LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_DN, false);

	//UP
//	Chip_IOCON_PinMux(LPC_IOCON,LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_UP, IOCON_MODE_PULLDOWN, IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_UP , IOCON_MODE_PULLDOWN |IOCON_FUNC0);

	/* Set the PIO_11 as input */
//	Chip_GPIO_WriteDirBit(LPC_GPIO, LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_UP, false);

	Chip_GPIO_WriteDirBit(LPC_GPIO, LIMIT_SWITCHES_PORT, LIMIT_SWITCHES_PIN_UP, false);
}


/* Sets the state of a board Pin to on or off */
void HAL_LimitSwitches_PINS_Write(uint8_t ui8PinNumber, Bool On)
{

	if (ui8PinNumber == LIMIT_SWITCHES_PIN_DN)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, LIMIT_SWITCHES_PORT, ui8PinNumber, On);
	}

	if (ui8PinNumber == LIMIT_SWITCHES_PIN_UP)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, LIMIT_SWITCHES_PORT, ui8PinNumber, On);
	}
}


/* Returns the current state of a board Pin */
Bool HAL_LimitSwitches_PINS_Read(uint8_t ui8PinNumber)
{
	Bool state = false;

	if (ui8PinNumber == LIMIT_SWITCHES_PIN_DN) {
		state = Chip_GPIO_GetPinState(LPC_GPIO, LIMIT_SWITCHES_PORT, ui8PinNumber);
	}

	if (ui8PinNumber == LIMIT_SWITCHES_PIN_UP) {
		state = Chip_GPIO_GetPinState(LPC_GPIO, LIMIT_SWITCHES_PORT, ui8PinNumber);
	}

	return state;
}

