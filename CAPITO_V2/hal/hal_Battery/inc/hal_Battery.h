/*
 * hal_Battery.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_BATTERY_INC_HAL_BATTERY_H_
#define HAL_HAL_BATTERY_INC_HAL_BATTERY_H_


/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
#define BATTERY_PORT                 1
#define BATTERY_PIN                  30

#define ADC_REF_VOLTAGE        3.3
#define ADC_RESOLUTION        4095

#ifdef BOARD_NXP_LPCXPRESSO_1769
#define _ADC_CHANNLE ADC_CH4
#else
#define _ADC_CHANNLE ADC_CH2
#endif
#define _LPC_ADC_ID LPC_ADC
#define _LPC_ADC_IRQ ADC_IRQn
#define _GPDMA_CONN_ADC GPDMA_CONN_ADC

//static char WelcomeMenu[] = "\r\nHello NXP Semiconductors \r\n"
//							"ADC DEMO \r\n"
//#if defined(CHIP_LPC175X_6X)
//							"Sample rate : 200kHz \r\n"
//#else
//							"Sample rate : 400kHz \r\n"
//#endif
//							"Press \'c\' to continue or \'x\' to quit\r\n"
//							"Press \'o\' or \'p\' to set Sample rate\r\n"
//							"Press \'b\' to ENABLE or DISABLE Burst Mode\r\n";

/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/

extern uint16_t g_ui16dataADC;
extern uint8_t g_ui8BatteryPercentage;

/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/

void HAL_Battery_Init(void);
void HAL_Battery_ADCPolling_Read(uint16_t *ui16dataADC);
uint8_t HAL_Battery_ADCPercentage(uint16_t ui16adcvalue);

#endif /* HAL_HAL_BATTERY_INC_HAL_BATTERY_H_ */
