/*
 * hal_SensorThreshold.c
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
#include "ssp_17xx_40xx.h"
#include "string.h"
#include "hal_SensorThreshold.h"
/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/
/* Private defines */
#define HAL_DIGITALPOTEN_UPDATE_YES 	0x0U
#define HAL_DIGITALPOTEN_UPDATE_NO 		0x1U

#define HAL_DIGITALPOTEN_WIPER_VALUE_MIN	0U
#define HAL_DIGITALPOTEN_WIPER_VALUE_MAX	256U
#define HAL_DIGITALPOTEN_WIPER_MAX	2U

#define HAL_DIGITALPOTEN_ASSERT_WIPER_VALUE(VALUE) 												\
		if ((VALUE > HAL_DIGITALPOTEN_WIPER_VALUE_MAX) || (VALUE < HAL_DIGITALPOTEN_WIPER_VALUE_MIN)) 	\
		{																								\
			return HAL_DIGITALPOTEN_STATUS_ERROR_WIPER_VALUE;											\
		}

#define HAL_DIGITALPOTEN_ASSERT_WIPER_ID(ID) 		\
		if (ID > HAL_DIGITALPOTEN_WIPER_ID_MAX) 			\
		{													\
			return HAL_DIGITALPOTEN_STATUS_ERROR_WIPER_ID;	\
		}


// Register Addresses
#define HAL_DIGITALPOTEN_REG_ADD_WIPER_0		0x00U
#define HAL_DIGITALPOTEN_REG_ADD_WIPER_1		0x01U
#define HAL_DIGITALPOTEN_REG_ADD_TCON			0x04U
#define HAL_DIGITALPOTEN_REG_ADD_STATUS			0x05U


// Commands
#define HAL_DIGITALPOTEN_CMD_16_READ		0x3U
#define HAL_DIGITALPOTEN_CMD_16_WRITE		0x0U
#define HAL_DIGITALPOTEN_CMD_8_INCREMENT	0x1U
#define HAL_DIGITALPOTEN_CMD_8_DECREMENT	0x2U

/* Private typedef */
//typedef struct
//{
//	uint16_t ui16Data:9;
//	uint16_t ui16Reserved:1;
//	uint16_t ui16Command:2;
//	uint16_t ui16Address:4;
//}HalDigitalPotenHalfWordCommand_st;

typedef struct
{
	uint16_t ui16Data2Msb:2;
	uint16_t ui16Command:2;
	uint16_t ui16Address:4;
	uint16_t ui16Data8Lsb:8;
}HalDigitalPotenHalfWordCommand_st;


typedef struct
{
	uint8_t ui8Data:2;
	uint8_t ui8Command:2;
	uint8_t ui8Address:4;
}HalDigitalPotenByteCommand_st;



typedef struct
{
	uint16_t					ui16WiperValue;
	uint8_t						ui8Update;
} HalDigitalPotenWiper_st;


typedef struct
{
	LPC_SSP_T*          hspi;
	LPC_GPIO_TypeDef*         GPIOx_ChipSelect;
	uint16_t 					GPIO_Pin_ChipSelect;
	HalDigitalPotenWiper_st		astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_MAX];
} HalDigitalPoten_st;





/* Private Variables */
volatile static HalDigitalPoten_st gs_stDigitalPoten;
static SSP_ConfigFormat ssp_format;

/**
 * @brief	SSP interrupt handler sub-routine
 * @return	Nothing
 */
//void SSP1_IRQHandler(void)
//{
	//	Chip_SSP_Int_Disable(LPC_SSP1);	/* Disable all interrupt */
	//	if (SSP_DATA_BYTES(ssp_format.bits) == 1) {
	//		Chip_SSP_Int_RWFrames8Bits(LPC_SSP, &xf_setup);
	//	}
	//	else {
	//		Chip_SSP_Int_RWFrames16Bits(LPC_SSP, &xf_setup);
	//	}
	//
	//	if ((xf_setup.rx_cnt != xf_setup.length) || (xf_setup.tx_cnt != xf_setup.length)) {
	//		Chip_SSP_Int_Enable(LPC_SSP);	/* enable all interrupts */
	//	}
	//	else {
	//		isXferCompleted = 1;
	//	}
//}



void SSP_CS_High (uint8_t ui8CSPinNum)
{
	/* SSEL is GPIO, set to high.  */
	LPC_GPIO0->FIOPIN |= (1 << ui8CSPinNum);
}
void SSP_CS_Low (uint8_t ui8CSPinNum)
{
	/* SSEL is GPIO, set to low.  */
	LPC_GPIO0->FIOPIN &= ~(1 << ui8CSPinNum);
}
void HAL_SensorThreshold_PINS_Configure(void)
{
	// Configure Sensor 2 hit pin (P0.5)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_HIT_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	// Configure sensor 2 NCS pin (P0.16)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, IOCON_MODE_INACT, IOCON_FUNC2);

	// Configure Sensor 1 hit pin (P0.4)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_HIT_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	// Configure Sensor 2  NCS pin (P0.6)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, IOCON_MODE_INACT, IOCON_FUNC2);


}
/*
 * \breif			This function is used to Initialize Digital Potentiometer Module
 * \Sync/Async		Synchronous
 * \Reentrancy		Non-Reentrant
 * \param			hspi				pointer to memory used to hold spi instance settings
 * \param			ui16ThrottleValue	Initial Value for Throttle
 * \param			ui16BrakeValue		Initial Value for Brake
 * \return			HalDigitalPotenStatus_et	refer to "hal_DigitalPoten.h" for more details
 * */
HalDigitalPotenStatus_et HAL_SensorThreshold12_SPI1_Init(LPC_SSP_T*     hspi,uint8_t ui8CSPinNum)
{

	HalDigitalPotenStatus_et etHalDigitalPotenStatus = HAL_DIGITALPOTEN_STATUS_SUCCESS;


	gs_stDigitalPoten.GPIOx_ChipSelect = SENSOR_THRESHOLD_SPI1_PORT;
	if( ui8CSPinNum==SENSOR1_POTEN_NCS_PIN)
	{
		gs_stDigitalPoten.GPIO_Pin_ChipSelect = ui8CSPinNum;
	}
	else if (ui8CSPinNum==SENSOR2_POTEN_NCS_PIN)
	{
		gs_stDigitalPoten.GPIO_Pin_ChipSelect = ui8CSPinNum;
	}

	gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_0].ui16WiperValue = 0;
	gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_1].ui16WiperValue = 0;
	gs_stDigitalPoten.hspi = hspi;

	gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_0].ui8Update= HAL_DIGITALPOTEN_UPDATE_NO;
	gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_1].ui8Update = HAL_DIGITALPOTEN_UPDATE_NO;

	/* SSP initialization */
	HAL_SensorThreshold_PINS_Configure();

	Board_SSP_Init(LPC_SSP1);

	Chip_SSP_Init(LPC_SSP1);

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP1, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);

	Chip_SSP_Enable(LPC_SSP1);

	if( ui8CSPinNum==SENSOR1_POTEN_NCS_PIN)
	{
		// set Potentiometer Chip select
		SSP_CS_High (ui8CSPinNum);
		// Configure Digital Potentiometer TCON Register : MCP4251-103E/SL
		HAL_SensorThreshold12_WriteReg(ui8CSPinNum,0xFF , HAL_DIGITALPOTEN_CMD_16_WRITE , HAL_DIGITALPOTEN_REG_ADD_TCON );

	}
	else if (ui8CSPinNum == SENSOR2_POTEN_NCS_PIN)
	{
		SSP_CS_High (ui8CSPinNum);
		// Configure Digital Potentiometer TCON Register : MCP4251-103E/SL
		HAL_SensorThreshold12_WriteReg(ui8CSPinNum,0xFF , HAL_DIGITALPOTEN_CMD_16_WRITE , HAL_DIGITALPOTEN_REG_ADD_TCON );

	}
	return etHalDigitalPotenStatus;
}

/*
 * \breif			This function is used to write data to Digital Potentiometer register
 * \Sync/Async		Synchronous
 * \Reentrancy		Non-Reentrant
 * \param			ui16Data			Data to be written
 * \param			ui8Command			Command
 * \param			ui8RegAddress		Register Address
 * \return			HalDigitalPotenStatus_et	refer to "hal_DigitalPoten.h" for more details
 * */


HalDigitalPotenStatus_et  HAL_SensorThreshold12_WriteReg (uint8_t ui8CSPinNum, uint16_t ui16Data , uint8_t ui8Command , uint8_t ui8RegAddress)
{
	HalDigitalPotenStatus_et etHalDigitalPotenStatus = HAL_DIGITALPOTEN_STATUS_SUCCESS;

	HalDigitalPotenHalfWordCommand_st stDigitalPotenHalfWordCommand_st;

	// Reset Command
	memset((void *)&stDigitalPotenHalfWordCommand_st , 0 , sizeof(HalDigitalPotenHalfWordCommand_st));

	// Configure Digital Potentiometer Chip : MCP4251-103E/SL
	stDigitalPotenHalfWordCommand_st.ui16Data2Msb = ((0x0300 & ui16Data) >> 0x8U);
	stDigitalPotenHalfWordCommand_st.ui16Data8Lsb = (0x00FF & ui16Data);
	stDigitalPotenHalfWordCommand_st.ui16Command = ui8Command;
	stDigitalPotenHalfWordCommand_st.ui16Address = ui8RegAddress;

	if (ui8CSPinNum==SENSOR1_POTEN_NCS_PIN)
	{
		// Reset Potentiometer Chip select
		SSP_CS_Low(ui8CSPinNum);

		//		HAL_SPI_Transmit(gs_stDigitalPoten.hspi, (uint8_t *)&stDigitalPotenHalfWordCommand_st , sizeof(stDigitalPotenHalfWordCommand_st), HAL_MAX_DELAY);

		Chip_SSP_WriteFrames_Blocking(LPC_SSP1, (uint8_t *)&stDigitalPotenHalfWordCommand_st,  sizeof(stDigitalPotenHalfWordCommand_st));

		// set Potentiometer Chip select
		SSP_CS_High(ui8CSPinNum);
	}
	else if (ui8CSPinNum==SENSOR2_POTEN_NCS_PIN)
	{
		// Reset Potentiometer Chip select
		SSP_CS_Low(ui8CSPinNum);

		Chip_SSP_WriteFrames_Blocking(LPC_SSP1, (uint8_t *)&stDigitalPotenHalfWordCommand_st,  sizeof(stDigitalPotenHalfWordCommand_st));
		//		HAL_SPI_Transmit(gs_stDigitalPoten.hspi, (uint8_t *)&stDigitalPotenHalfWordCommand_st , sizeof(stDigitalPotenHalfWordCommand_st), HAL_MAX_DELAY);

		// set Potentiometer Chip select
		SSP_CS_High(ui8CSPinNum);
	}

	return etHalDigitalPotenStatus;
}

/*
 * \breif			This function is used to Write set Wiper Value
 * \Sync/Async		Synchronous
 * \Reentrancy		Non-Reentrant
 * \param			etDigitalPotenWiper		refer to "hal_DigitalPoten.h" for more details
 * \param			ui16Value					Wiper value [0-256]
 * \return			HalDigitalPotenStatus_et	refer to "hal_DigitalPoten.h" for more details
 * */

HalDigitalPotenStatus_et	HAL_SensorThreshold12_SetWiper (HalDigitalPotenWiperId_et etDigitalPotenWiper,uint16_t ui16Value)
{
	HalDigitalPotenStatus_et etHalDigitalPotenStatus = HAL_DIGITALPOTEN_STATUS_SUCCESS;

	// Assert Wiper Value
	HAL_DIGITALPOTEN_ASSERT_WIPER_VALUE(ui16Value);

	// Assert Wiper Id
	HAL_DIGITALPOTEN_ASSERT_WIPER_ID(etDigitalPotenWiper );



	switch (etDigitalPotenWiper)
	{
	case HAL_DIGITALPOTEN_WIPER_ID_0:
		gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_0].ui16WiperValue  = ui16Value;
		gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_0].ui8Update= HAL_DIGITALPOTEN_UPDATE_YES;
		break;

	case HAL_DIGITALPOTEN_WIPER_ID_1:
		gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_1].ui16WiperValue  = ui16Value;
		gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_1].ui8Update= HAL_DIGITALPOTEN_UPDATE_YES;
		break;

	default:
		break;
	}

	return etHalDigitalPotenStatus;
}

/*
 * \breif			This function is used to Write Get Wiper Value
 * \Sync/Async		Synchronous
 * \Reentrancy		Non-Reentrant
 * \param			etDigitalPotenWiper		refer to "hal_DigitalPoten.h" for more details
 * \param			pui16Value				pointer to memory used to save value of wiper
 * \return			HalDigitalPotenStatus_et	refer to "hal_DigitalPoten.h" for more details
 * */

HalDigitalPotenStatus_et	HAL_SensorThreshold12_GetWiper (HalDigitalPotenWiperId_et etDigitalPotenWiper ,uint16_t *pui16Value )
{
	HalDigitalPotenStatus_et etHalDigitalPotenStatus = HAL_DIGITALPOTEN_STATUS_SUCCESS;


	// Assert Wiper Id
	HAL_DIGITALPOTEN_ASSERT_WIPER_ID(etDigitalPotenWiper );


	switch (etDigitalPotenWiper)
	{
	case HAL_DIGITALPOTEN_WIPER_ID_0:
		*pui16Value = gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_0].ui16WiperValue ;
		break;

	case HAL_DIGITALPOTEN_WIPER_ID_1:
		*pui16Value = gs_stDigitalPoten.astDigitalPotenWiper[HAL_DIGITALPOTEN_WIPER_ID_1].ui16WiperValue ;
		break;

	default:
		break;
	}

	return etHalDigitalPotenStatus;
}

/*
 * \breif			This function is used to Update Wiper Value
 * \Sync/Async		Synchronous
 * \Reentrancy		Non-Reentrant
 * \param			None
 * \return			HalDigitalPotenStatus_et	refer to "hal_DigitalPoten.h" for more details
 * */

HalDigitalPotenStatus_et	HAL_SensorThreshold12_Update ( uint8_t ui8CSPinNum)
{
	HalDigitalPotenStatus_et etHalDigitalPotenStatus = HAL_DIGITALPOTEN_STATUS_SUCCESS;
	int32_t i32;
	uint8_t ui8Address = 0;


	for (i32 = 0 ; i32 < HAL_DIGITALPOTEN_WIPER_ID_MAX ; i32++)
	{
		if (gs_stDigitalPoten.astDigitalPotenWiper[i32].ui8Update == HAL_DIGITALPOTEN_UPDATE_YES)
		{
			gs_stDigitalPoten.astDigitalPotenWiper[i32].ui8Update = HAL_DIGITALPOTEN_UPDATE_NO;

			switch(i32)
			{
			case HAL_DIGITALPOTEN_WIPER_ID_0:
				ui8Address = HAL_DIGITALPOTEN_REG_ADD_WIPER_0;
				break;

			case HAL_DIGITALPOTEN_WIPER_ID_1:
				ui8Address = HAL_DIGITALPOTEN_REG_ADD_WIPER_1;
				break;

			default:
				break;
			}


			HAL_SensorThreshold12_WriteReg(ui8CSPinNum, gs_stDigitalPoten.astDigitalPotenWiper[i32].ui16WiperValue ,
					HAL_DIGITALPOTEN_CMD_16_WRITE ,
					ui8Address );
		}
	}


	return etHalDigitalPotenStatus;
}
