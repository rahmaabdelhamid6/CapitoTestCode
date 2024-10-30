/*
 * app_CapitoTest.c
 *
 *  Created on: Jan 9, 2024
 *      Author: Rahma
 */


/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "lpc_types.h"
#include "board.h"
#include "LPC17xx.h"
#include "string.h"
#include "hal_Debug.h"
#include "hal_Battery.h"
#include "hal_EEPROM.h"
#include "hal_ExtraUart.h"
#include "hal_LimitSwitches.h"
#include "hal_Modem.h"
#include "hal_SensorThreshold.h"  //cs , hit
#include "hal_SmokeBox.h"
#include "hal_Target.h"
#include "hal_TargetIlluminator.h"
#include "app_CapitoHelper.h"
#include "app_CapitoTest.h"


/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/

uint8_t g_aui8DebugRxBuff[CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES]={0};
uint8_t g_aui8DebugTxBuff[CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES]={0x55 , 0x0A , 0x40 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,  0x00 , 0x9F};
uint8_t g_ui8MsgType=0;
//uint32_t g_ui32PCBIdParsing = 0;
//uint32_t g_ui32PCBID= 0x00000001;
uint32_t g_ui32PCBIDRead= 0;
//SSP DATA , EXTRA UART , MODEM
uint8_t g_aui8TxBuffer[7] = {1,2,3,4,5,6,7};
uint8_t g_aui8RxBuffer[7]={0};
//spi
SSP_ConfigFormat ssp_format;
Chip_SSP_DATA_SETUP_T  sspDATA;

uint8_t g_ui8Read = 0 ;

/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void delay_ms(uint32_t milliseconds)
{
	// Calculate the number of iterations required for the desired delay
	//	uint32_t iterations = milliseconds * (SystemCoreClock / 120000);
	uint32_t iterations = milliseconds * (SystemCoreClock / 1000);
	uint32_t i=0;

	// Perform the delay by executing the loop
	for ( i = 0; i < iterations; i++) {
		// Do nothing
	}
}
void APP_SSPTEST_PINS_Configure(void)
{
	//SPI INIT
	Board_SSP_Init(LPC_SSP1);
	Chip_SSP_Init(LPC_SSP1);
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_BITS_8;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP1, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_Enable(LPC_SSP1);
	LPC_GPIO0->FIOPIN |= (1 << 6);


	// Configure Sensor 2 hit pin (P0.5)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_HIT_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	/* Set the PIO0_5 as INPUT */
	Chip_GPIO_WriteDirBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_HIT_PIN, false);

	// Configure sensor 2 NCS pin (P0.16)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	/* Set the PIO0_16 as OUTPUT */
	Chip_GPIO_WriteDirBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, true);

	// Configure Sensor 1 hit pin (P0.4)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_HIT_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	/* Set the PIO0_4 as INPUT */
	Chip_GPIO_WriteDirBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_HIT_PIN, false);

	// Configure Sensor 1  NCS pin (P0.6)
	Chip_IOCON_PinMux(LPC_IOCON, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, IOCON_MODE_INACT, IOCON_FUNC0);

	/* Set the PIO0_6 as OUTPUT */
	Chip_GPIO_WriteDirBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, true);
}

CapitoStatus_et APP_CapitoTest_Init (void)
{

	CapitoStatus_et  etCapitoStatus = CAPITO_SUCCESS;
	CapitoTestSequence_et etCapitoTestSequence=BATTERY_LEVEL_TEST;
	uint8_t i =0;
	uint8_t ui8DebugRxBuffSize=0;
	sspDATA.tx_data=g_aui8TxBuffer;
	sspDATA.rx_data=g_aui8RxBuffer;
	sspDATA.length=7;
	sspDATA.rx_cnt=0;
	sspDATA.tx_cnt=0;

	static uint8_t ui8DataCheck = 0; // spi data, eeprom , modem
	static uint8_t ui8DataCheckTrue = 0; //hit12
	static uint8_t ui8DataCheckFalse = 0 ; //hit12
	static uint8_t ui8DataWrite=0;


	NVIC_EnableIRQ(UART3_IRQn);
	Chip_UART_IntEnable(LPC_UART3,  UART_IER_RBRINT|UART_IER_RLSINT);
	while (rxComplete == FALSE);
	ui8DebugRxBuffSize=HAL_Debug_UART3_ReadBlocking(LPC_UART3,g_aui8DebugRxBuff,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
	rxComplete = FALSE;

//	ui8DebugRxBuffSize=HAL_Debug_UART3_ReadBlocking(LPC_UART3,g_aui8DebugRxBuff,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);


	APP_CapitoHelper_ParseMsg ( g_aui8DebugRxBuff , ui8DebugRxBuffSize ,&g_ui8MsgType,&g_ui32PCBIdParsing);

	switch(g_ui8MsgType)
	{
	case CAPITO_HELPER_SET_PCB_ID_REQ_MSG :

		//hal hna hab3t id  f data 6 byte????
		HAL_EEPROM_I2C1_WritePage( EEPROM_ADDR , EEPROM_MEMORY_ID_ADD , (uint8_t *)&g_ui32PCBIdParsing, 4);
		delay_ms(10);
		HAL_EEPROM_I2C1_ReadPage( EEPROM_ADDR , EEPROM_MEMORY_ID_ADD ,  (uint8_t *)&g_ui32PCBIDRead, 4);

		if (g_ui32PCBIDRead == g_ui32PCBIdParsing )
		{
			memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
			APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SET_PCB_ID_ACK_MSG, 0, 0);
			HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
		}

		delay_ms(10);

		break;

	case CAPITO_HELPER_START_TEST_REQ_MSG:

		g_ui32PCBIDRead=0;
		HAL_EEPROM_I2C1_ReadPage(EEPROM_ADDR, EEPROM_MEMORY_ID_ADD,  (uint8_t *)&g_ui32PCBIDRead, 4);
		APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_TEST_STARTED_REPLY_MSG, 0, g_ui32PCBIDRead);
		HAL_Debug_UART3_SendBlocking(LPC_UART3,g_aui8DebugTxBuff,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);


		delay_ms(100);
		etCapitoTestSequence = BATTERY_LEVEL_TEST ;
		while (g_ui8MsgType == CAPITO_HELPER_START_TEST_REQ_MSG)
		{
			switch (etCapitoTestSequence)
			{
			case BATTERY_LEVEL_TEST :

				HAL_Battery_ADCPolling_Read(&g_ui16dataADC);
				g_ui8BatteryPercentage=HAL_Battery_ADCPercentage(g_ui16dataADC);
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_BATTERY_LEVEL_TEST_RESULT_MSG, g_ui8BatteryPercentage, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				delay_ms(100);
				etCapitoTestSequence = SPI_CLK_TEST ;

				break;

			case SPI_CLK_TEST :

				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_CLK_TEST_STARTED_MSG, g_ui8BatteryPercentage, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				APP_SSPTEST_PINS_Configure();
				for ( i = 0; i < 4; i++)
				{
					Chip_SSP_WriteFrames_Blocking(LPC_SSP1,g_aui8TxBuffer,7);
					delay_ms(150);
					Chip_SSP_WriteFrames_Blocking(LPC_SSP1,g_aui8TxBuffer,7);
					delay_ms(150);
				}

				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_CLK_TEST_ENDED_MSG, g_ui8BatteryPercentage, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				delay_ms(100);
				etCapitoTestSequence = SPI_DATA_TEST ;
				break;

			case SPI_DATA_TEST :

				APP_SSPTEST_PINS_Configure();
				Chip_SSP_RWFrames_Blocking(LPC_SSP1,&sspDATA);
				for ( i = 0; i < 7; i++)
				{
					if (g_aui8RxBuffer[i]!=g_aui8TxBuffer[i])
					{
						ui8DataCheck = TEST_FAILED;
						break;
					}
					else {
						ui8DataCheck = TEST_PASSED;
					}
				}
				if (ui8DataCheck == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_DATA_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				else if (ui8DataCheck == TEST_FAILED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_DATA_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				etCapitoTestSequence = SPI_CS1_CS2_TEST ;

				break;

			case SPI_CS1_CS2_TEST :

				APP_SSPTEST_PINS_Configure();
				//test started
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_CS1_CS2_TEST_STARTED_RESULT_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, false);
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, false);


				//CS1
				for(i=0; i<5; i++)
				{
					Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, true);
					delay_ms(50);
					Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, false);
					delay_ms(50);
				}

				delay_ms(10);

				//CS2
				for(i=0; i<5; i++)
				{
					Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, true);
					delay_ms(50);
					Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, false);
					delay_ms(50);
				}



				//test ended
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_CS1_CS2_TEST_ENDED_RESULT_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				delay_ms(100);
				etCapitoTestSequence = SPI_HIT1_TEST ;

				break;

			case SPI_HIT1_TEST :
				//hit 1 true
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, TRUE);
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT,SENSOR1_HIT_PIN, TRUE);
				g_ui8Read=Chip_GPIO_ReadPortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_HIT_PIN);
				if (g_ui8Read == 1 )
				{
					ui8DataCheckTrue= TEST_PASSED;
				}
				else
				{
					ui8DataCheckTrue= TEST_FAILED;
				}

				delay_ms(500);

				//HIT 1  false
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_POTEN_NCS_PIN, FALSE);
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT,SENSOR1_HIT_PIN, FALSE);
				g_ui8Read=Chip_GPIO_ReadPortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR1_HIT_PIN);
				if (g_ui8Read == 0 )
				{
					ui8DataCheckFalse= TEST_PASSED;
				}
				else
				{
					ui8DataCheckFalse= TEST_FAILED;
				}

				delay_ms(250);

				if ( ui8DataCheckTrue == TEST_PASSED && ui8DataCheckFalse == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_HIT1_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				else
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_HIT1_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				delay_ms(100);
				etCapitoTestSequence = SPI_HIT2_TEST ;
				break;

			case SPI_HIT2_TEST :

				//HIT 2  true
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, TRUE);
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT,SENSOR2_HIT_PIN, TRUE);
				g_ui8Read=Chip_GPIO_ReadPortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_HIT_PIN);
				if (g_ui8Read == 1 )
				{
					ui8DataCheckTrue= TEST_PASSED;
				}
				else
				{
					ui8DataCheckTrue= TEST_FAILED;
				}

				delay_ms(500);

				//HIT 2  false
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_POTEN_NCS_PIN, FALSE);
				Chip_GPIO_WritePortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT,SENSOR2_HIT_PIN, FALSE);
				g_ui8Read=Chip_GPIO_ReadPortBit(LPC_GPIO, SENSOR_THRESHOLD_SPI1_PORT, SENSOR2_HIT_PIN);
				if (g_ui8Read == 0 )
				{
					ui8DataCheckFalse= TEST_PASSED;
				}
				else
				{
					ui8DataCheckFalse= TEST_FAILED;
				}

				delay_ms(250);

				if ( ui8DataCheckTrue == TEST_PASSED && ui8DataCheckFalse == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_HIT2_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				else
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SPI_HIT2_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				delay_ms(100);
				etCapitoTestSequence = EXTRA_UART_TEST ;
				break;

			case EXTRA_UART_TEST :
				memset(g_aui8RxBuffer,0,7);
				HAL_ExtraUart_UART2_SendBlocking(LPC_UART2, g_aui8TxBuffer, 7);
				delay_ms(10);
				HAL_ExtraUart_UART2_Read(LPC_UART2, g_aui8RxBuffer, 7);
				for ( i = 0; i < 7; i++)
				{
					if (g_aui8RxBuffer[i]!=g_aui8TxBuffer[i])
					{
						ui8DataCheck = TEST_FAILED;
						break;
					}
					else {
						ui8DataCheck = TEST_PASSED;
					}
				}
				if (ui8DataCheck == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_EXTRA_UART_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				else if (ui8DataCheck == TEST_FAILED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_EXTRA_UART_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				delay_ms(100);
				etCapitoTestSequence = SMOKE_BOX_TEST ;
				break;

			case SMOKE_BOX_TEST :

				//test started
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SMOKE_BOX_TEST_STARTED_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				//smoke box 12V
				for(i=0; i<5; i++)
				{
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_12V_PIN_PULSE, TRUE);
					delay_ms(50);
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_12V_PIN_PULSE, FALSE);
					delay_ms(50);
				}

				delay_ms(10);

				//smoke box 5V
				for(i=0; i<5; i++)
				{
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_5V_PIN_PULSE, TRUE);
					delay_ms(50);
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_5V_PIN_PULSE, FALSE);
					delay_ms(50);
				}

				//test ended
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_SMOKE_BOX_TEST_ENDED_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				delay_ms(100);
				etCapitoTestSequence = MODEM_TEST ;
				break;

			case MODEM_TEST :
				memset(g_aui8RxBuffer,0,7);
				HAL_Modem_UART1_SendBlocking(LPC_UART1, g_aui8TxBuffer, 7);
				delay_ms(10);
				HAL_Modem_UART1_Read(LPC_UART1, g_aui8RxBuffer, 7);
				for ( i = 0; i < 7; i++)
				{
					if (g_aui8RxBuffer[i]!=g_aui8TxBuffer[i])
					{
						ui8DataCheck = TEST_FAILED;
						break;
					}
					else {
						ui8DataCheck = TEST_PASSED;
					}
				}
				if (ui8DataCheck == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_MODEM_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				else if (ui8DataCheck == TEST_FAILED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_MODEM_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				delay_ms(100);
				etCapitoTestSequence = LIMIT_UP_TEST ;
				break;

			case LIMIT_UP_TEST :

				//5V with up switch (true)
				HAL_SmokeBox_PINS_Write(SMOKE_BOX_5V_PIN_PULSE, TRUE);
				HAL_LimitSwitches_PINS_Write(LIMIT_SWITCHES_PIN_UP, TRUE);
				delay_ms(10);
				g_ui8Read=HAL_LimitSwitches_PINS_Read(LIMIT_SWITCHES_PIN_UP);
				if (g_ui8Read == 1 )
				{
					ui8DataCheckTrue= TEST_PASSED;
				}
				else
				{
					ui8DataCheckTrue= TEST_FAILED;
				}

				delay_ms(500);

				//5V with up switch (true)
				HAL_SmokeBox_PINS_Write(SMOKE_BOX_5V_PIN_PULSE, FALSE);
				HAL_LimitSwitches_PINS_Write(LIMIT_SWITCHES_PIN_UP, FALSE);
				delay_ms(10);
				g_ui8Read=HAL_LimitSwitches_PINS_Read(LIMIT_SWITCHES_PIN_UP);
				if (g_ui8Read == 0 )
				{
					ui8DataCheckFalse= TEST_PASSED;
				}
				else
				{
					ui8DataCheckFalse= TEST_FAILED;
				}

				delay_ms(250);

				if ( ui8DataCheckTrue == TEST_PASSED && ui8DataCheckFalse == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_LIMIT_UP_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				else
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_LIMIT_UP_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				delay_ms(100);
				etCapitoTestSequence = LIMIT_DN_TEST ;
				break;

			case LIMIT_DN_TEST :

				//12V with DN switch (true)
				HAL_SmokeBox_PINS_Write(SMOKE_BOX_12V_PIN_PULSE, TRUE);
				HAL_LimitSwitches_PINS_Write(LIMIT_SWITCHES_PIN_DN, TRUE);
				delay_ms(10);
				g_ui8Read=HAL_LimitSwitches_PINS_Read(LIMIT_SWITCHES_PIN_DN);
				if (g_ui8Read == 1 )
				{
					ui8DataCheckTrue= TEST_PASSED;
				}
				else
				{
					ui8DataCheckTrue= TEST_FAILED;
				}

				delay_ms(500);

				//12V with DN switch (true)
				HAL_SmokeBox_PINS_Write(SMOKE_BOX_12V_PIN_PULSE, FALSE);
				HAL_LimitSwitches_PINS_Write(LIMIT_SWITCHES_PIN_DN, FALSE);
				delay_ms(10);
				g_ui8Read=HAL_LimitSwitches_PINS_Read(LIMIT_SWITCHES_PIN_DN);
				if (g_ui8Read == 0 )
				{
					ui8DataCheckFalse= TEST_PASSED;
				}
				else
				{
					ui8DataCheckFalse= TEST_FAILED;
				}

				delay_ms(250);

				if ( ui8DataCheckTrue == TEST_PASSED && ui8DataCheckFalse == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_LIMIT_DN_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}
				else
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_LIMIT_DN_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				delay_ms(100);
				etCapitoTestSequence = EEPROM_TEST ;
				break;

			case EEPROM_TEST :

				ui8DataWrite = HAL_EEPROM_I2C1_WriteByte(EEPROM_ADDR,EEPROM_MEMORY_ADD,0xCD);
				delay_ms(10);
				HAL_EEPROM_I2C1_ReadByte(EEPROM_ADDR,EEPROM_MEMORY_ADD);

				if (g_aui8EEPROMRxBuffer[0]!= ui8DataWrite)
				{
					ui8DataCheck = TEST_FAILED;

				}
				else
				{
					ui8DataCheck = TEST_PASSED;
				}

				if (ui8DataCheck == TEST_PASSED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_EEPROM_TEST_RESULT_MSG, TEST_PASSED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				else if (ui8DataCheck == TEST_FAILED)
				{
					memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
					APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_EEPROM_TEST_RESULT_MSG, TEST_FAILED, 0);
					HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				}

				delay_ms(100);
				etCapitoTestSequence = TARGET_ILLMINATOR_TEST ;
				break;

			case TARGET_ILLMINATOR_TEST :

				//test started
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_TARGET_ILLMINATOR_TEST_STARTED_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				for(i=0; i<5; i++)
				{
					//TargetIlluminator true
					HAL_TargetIlluminator_PIN_Write(TARGET_ILLUMINATOR_PIN, TRUE);
					delay_ms(50);
					//TargetIlluminator false
					HAL_TargetIlluminator_PIN_Write(TARGET_ILLUMINATOR_PIN, FALSE);
					delay_ms(50);
				}
				delay_ms(10);

				//test ended
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_TARGET_ILLMINATOR_TEST_ENDED_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				delay_ms(100);
				etCapitoTestSequence = TARGET_MOVE_UP_DN_TEST ;
				break;

			case TARGET_MOVE_UP_DN_TEST :

				//test started
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_TARGET_MOVE_UP_DN_TEST_STARTED_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				//12V (True) -> MV_UP (true) 8 SEC -> MV_UP (false ) 4 SEC -> 12V (false)
				for(i=0; i<5; i++)
				{
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_12V_PIN_PULSE, TRUE);
					//move up 1
					HAL_Target_PINS_Write(TARGET_PIN_MV_UP, TRUE);
					delay_ms(50);
					// move up 0
					HAL_Target_PINS_Write(TARGET_PIN_MV_UP, FALSE);
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_12V_PIN_PULSE, FALSE);
					delay_ms(50);
				}

				//5V (True) -> MV_DN (true) 8 SEC -> MV_DN (false ) 4 SEC -> 5V (false)
				//move down 1
				for(i=0; i<5; i++)
				{
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_5V_PIN_PULSE, TRUE);
					HAL_Target_PINS_Write(TARGET_PIN_MV_DN, TRUE);
					delay_ms(50);
					//move down 0
					HAL_Target_PINS_Write(TARGET_PIN_MV_DN, FALSE);
					HAL_SmokeBox_PINS_Write(SMOKE_BOX_5V_PIN_PULSE, FALSE);
					delay_ms(50);
				}

				//test ended
				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_TARGET_MOVE_UP_DN_TEST_ENDED_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				delay_ms(100);
				etCapitoTestSequence = END_OF_TEST ;
				break;

			case END_OF_TEST :

				memset(g_aui8DebugTxBuff,0,CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);
				APP_PCHelper_ConstructMsg(g_aui8DebugTxBuff, CAPITO_HELPER_END_OF_TEST_ACK_MSG, 0, 0);
				HAL_Debug_UART3_SendBlocking(LPC_UART3, g_aui8DebugTxBuff, CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES);

				delay_ms(10);
				etCapitoTestSequence = BATTERY_LEVEL_TEST ;
				g_ui8MsgType= CAPITO_HELPER_SET_PCB_ID_REQ_MSG;
				break;

			}  //end of switch parsing CAPITO_HELPER_START_TEST_REQ_MSG

		}
		break;
	}
	return etCapitoStatus;
}
