/*
 * app_CapitoTest.h
 *
 *  Created on: Jan 9, 2024
 *      Author: Rahma
 */

#ifndef APP_APP_CAPITOTEST_INC_APP_CAPITOTEST_H_
#define APP_APP_CAPITOTEST_INC_APP_CAPITOTEST_H_

#include "app_CapitoHelper.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define TEST_PASSED   0
#define TEST_FAILED  1
#define EEPROM_MEMORY_ID_ADD    0x00
#define EEPROM_MEMORY_ADD       0x07


/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	CAPITO_SUCCESS,
	CAPITO_ERROR_CHECKSUM,
	CAPITO_INVALID_MSG_TYPE

}CapitoStatus_et;

typedef enum
{
	BATTERY_LEVEL_TEST,
	SPI_CLK_TEST,
	SPI_DATA_TEST,
	SPI_CS1_CS2_TEST,
	SPI_HIT1_TEST,
	SPI_HIT2_TEST,
	EXTRA_UART_TEST,
	SMOKE_BOX_TEST,
	MODEM_TEST,
	LIMIT_UP_TEST,
	LIMIT_DN_TEST,
	EEPROM_TEST,
	TARGET_ILLMINATOR_TEST,
	TARGET_MOVE_UP_DN_TEST,
	END_OF_TEST
}CapitoTestSequence_et;


/*******************************************************************************
 *                                Extern Variables                                 *
 *******************************************************************************/
extern uint8_t g_aui8DebugRxBuff[CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES];
extern uint8_t g_aui8DebugTxBuff[CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES];
extern uint8_t g_ui8MsgType;
extern uint32_t g_ui32PCBID;
extern uint32_t g_ui32PCBIDRead;
//SSP DATA , EXTRA UART , MODEM
extern uint8_t g_aui8TxBuffer[7] ;
extern uint8_t g_aui8RxBuffer[7];
//spi
extern SSP_ConfigFormat ssp_format;
extern Chip_SSP_DATA_SETUP_T  sspDATA;

extern uint8_t g_ui8Read ;

/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void delay_ms(uint32_t milliseconds);
void APP_SSPTEST_PINS_Configure(void);
CapitoStatus_et APP_CapitoTest_Init (void);

#endif /* APP_APP_CAPITOTEST_INC_APP_CAPITOTEST_H_ */
