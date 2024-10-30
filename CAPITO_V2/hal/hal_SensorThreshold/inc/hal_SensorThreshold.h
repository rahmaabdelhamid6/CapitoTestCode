/*
 * hal_SensorThreshold.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_SENSORTHRESHOLD_INC_HAL_SENSORTHRESHOLD_H_
#define HAL_HAL_SENSORTHRESHOLD_INC_HAL_SENSORTHRESHOLD_H_

/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
#define SENSOR_THRESHOLD_SPI1_PORT                   0
#define SENSOR2_HIT_PIN                              5
#define SENSOR2_POTEN_NCS_PIN                        16
#define SENSOR1_HIT_PIN                              4
#define SENSOR1_POTEN_NCS_PIN                        6




/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


typedef enum
{
	HAL_DIGITALPOTEN_STATUS_SUCCESS,
	HAL_DIGITALPOTEN_STATUS_ERROR,
	HAL_DIGITALPOTEN_STATUS_ERROR_WIPER_ID,
	HAL_DIGITALPOTEN_STATUS_ERROR_WIPER_VALUE
}HalDigitalPotenStatus_et;


typedef enum
{
	HAL_DIGITALPOTEN_WIPER_ID_0,
	HAL_DIGITALPOTEN_WIPER_ID_1,
	HAL_DIGITALPOTEN_WIPER_ID_MAX
}HalDigitalPotenWiperId_et;

/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/


/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void SSP_CS_High (uint8_t ui8CSPinNum);
void SSP_CS_Low (uint8_t ui8CSPinNum);
void HAL_SensorThreshold_PINS_Configure(void);
HalDigitalPotenStatus_et    HAL_SensorThreshold12_SPI1_Init(LPC_SSP_T*     hspi,uint8_t ui8CSPinNum);
HalDigitalPotenStatus_et    HAL_SensorThreshold12_WriteReg (uint8_t ui8CSPinNum, uint16_t ui16Data , uint8_t ui8Command , uint8_t ui8RegAddress);
HalDigitalPotenStatus_et	HAL_SensorThreshold12_SetWiper (HalDigitalPotenWiperId_et etDigitalPotenWiper,uint16_t ui16Value);
HalDigitalPotenStatus_et	HAL_SensorThreshold12_GetWiper (HalDigitalPotenWiperId_et etDigitalPotenWiper ,uint16_t *pui16Value );
HalDigitalPotenStatus_et	HAL_SensorThreshold12_Update ( uint8_t ui8CSPinNum);

#endif /* HAL_HAL_SENSORTHRESHOLD_INC_HAL_SENSORTHRESHOLD_H_ */
