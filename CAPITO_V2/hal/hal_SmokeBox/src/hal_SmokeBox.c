/*
 * hal_SmokeBox.c
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
#include "hal_SmokeBox.h"



/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/


/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/

void HAL_SmokeBox_PINS_Init(void)
{
	/* configure Pin PIO0_2 , Pin PIO0_3 as GPIO pin */

	//12V
//	Chip_IOCON_PinMux(LPC_IOCON,SMOKE_BOX_PORT, SMOKE_BOX_12V_PIN_PULSE, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, SMOKE_BOX_PORT, SMOKE_BOX_12V_PIN_PULSE , IOCON_MODE_INACT |IOCON_FUNC0);

	//5V
//	Chip_IOCON_PinMux(LPC_IOCON,SMOKE_BOX_PORT, SMOKE_BOX_5V_PIN_PULSE, IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, SMOKE_BOX_PORT, SMOKE_BOX_5V_PIN_PULSE , IOCON_MODE_INACT |IOCON_FUNC0);

	/* Set the PIO_2 as output */
	Chip_GPIO_WriteDirBit(LPC_GPIO, SMOKE_BOX_PORT, SMOKE_BOX_12V_PIN_PULSE, true);

	/* Set the PIO_3 as output */
	Chip_GPIO_WriteDirBit(LPC_GPIO, SMOKE_BOX_PORT, SMOKE_BOX_5V_PIN_PULSE, true);
}


/* Sets the state of a board Pin to on or off */
void HAL_SmokeBox_PINS_Write(uint8_t ui8PinNumber, Bool On)
{

	if (ui8PinNumber == SMOKE_BOX_12V_PIN_PULSE)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, SMOKE_BOX_PORT, ui8PinNumber, On);
	}

	if (ui8PinNumber == SMOKE_BOX_5V_PIN_PULSE)
	{
		Chip_GPIO_WritePortBit(LPC_GPIO, SMOKE_BOX_PORT, ui8PinNumber, On);
	}
}

/* Returns the current state of a board Pin */
Bool HAL_SmokeBox_PINS_Read(uint8_t ui8PinNumber)
{
	Bool state = false;

	if (ui8PinNumber == SMOKE_BOX_12V_PIN_PULSE) {
		state = Chip_GPIO_ReadPortBit(LPC_GPIO, SMOKE_BOX_PORT, ui8PinNumber);
	}

	if (ui8PinNumber == SMOKE_BOX_5V_PIN_PULSE) {
		state = Chip_GPIO_ReadPortBit(LPC_GPIO, SMOKE_BOX_PORT, ui8PinNumber);
	}

	return state;
}

