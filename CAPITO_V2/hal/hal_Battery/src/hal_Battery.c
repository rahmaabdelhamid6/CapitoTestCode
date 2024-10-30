/*
 * hal_Battery.c
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
#include "adc_17xx_40xx.h"
#include "hal_Battery.h"

/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/

uint16_t dataADC;
uint16_t g_ui16dataADC=0;
uint8_t g_ui8BatteryPercentage=0;
float ui8analogvalue=0;
ADC_CLOCK_SETUP_T ADCSetup;
/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void HAL_Battery_Init(void)
{
	Chip_IOCON_PinMux(LPC_IOCON,BATTERY_PORT, BATTERY_PIN, IOCON_MODE_INACT, IOCON_FUNC3);
	Chip_ADC_Init(_LPC_ADC_ID, &ADCSetup);
	Chip_ADC_EnableChannel(_LPC_ADC_ID, _ADC_CHANNLE, ENABLE);
}

void HAL_Battery_ADCPolling_Read(uint16_t *ui16dataADC)
{
	Chip_ADC_SetStartMode(_LPC_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	/* Waiting for A/D conversion complete */
	while (Chip_ADC_ReadStatus(_LPC_ADC_ID, _ADC_CHANNLE, ADC_DR_DONE_STAT) != SET) {}
	/* Read ADC value */
	Chip_ADC_ReadValue(_LPC_ADC_ID, _ADC_CHANNLE, &dataADC);

	*ui16dataADC=dataADC;
}

uint8_t HAL_Battery_ADCPercentage(uint16_t ui16adcvalue)
{
	//	float ui8analogvalue=0;
	//	ADCPolling_Read(g_ui16dataADC);
	uint8_t ui8BatteryPercentage=0;

	ui8analogvalue =((ui16adcvalue * ADC_REF_VOLTAGE)/ADC_RESOLUTION);

	ui8BatteryPercentage = ((ui8analogvalue/ADC_REF_VOLTAGE)*100);

	return ui8BatteryPercentage;
}
