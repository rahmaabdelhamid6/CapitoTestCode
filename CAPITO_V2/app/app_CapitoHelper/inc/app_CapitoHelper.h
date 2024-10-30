/*
 * app_CapitoHelper.h
 *
 *  Created on: Jan 8, 2024
 *      Author: Rahma
 */

#ifndef APP_APP_CAPITOHELPER_INC_APP_CAPITOHELPER_H_
#define APP_APP_CAPITOHELPER_INC_APP_CAPITOHELPER_H_

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/******************************************************************************/
// From PC TO CAPITO
#define			CAPITO_HELPER_START_TEST_REQ_MSG				0x40U
#define			CAPITO_HELPER_SET_PCB_ID_REQ_MSG			    0x50U

/******************************************************************************/
// From CAPITO TO PC

#define			CAPITO_HELPER_TEST_STARTED_REPLY_MSG                        0x41
#define			CAPITO_HELPER_SET_PCB_ID_ACK_MSG                            0x51
#define			CAPITO_HELPER_BATTERY_LEVEL_TEST_RESULT_MSG                 0x01
#define			CAPITO_HELPER_SPI_CLK_TEST_STARTED_MSG                      0x02
#define			CAPITO_HELPER_SPI_CLK_TEST_ENDED_MSG                        0x03
#define         CAPITO_HELPER_SPI_DATA_TEST_RESULT_MSG                      0x04
#define			CAPITO_HELPER_SPI_CS1_CS2_TEST_STARTED_RESULT_MSG           0x05
#define         CAPITO_HELPER_SPI_CS1_CS2_TEST_ENDED_RESULT_MSG             0x06
#define			CAPITO_HELPER_SPI_HIT1_TEST_RESULT_MSG                      0x07
#define         CAPITO_HELPER_SPI_HIT2_TEST_RESULT_MSG                      0x08
#define			CAPITO_HELPER_EXTRA_UART_TEST_RESULT_MSG                    0x09
#define         CAPITO_HELPER_SMOKE_BOX_TEST_STARTED_MSG                    0x0A
#define			CAPITO_HELPER_SMOKE_BOX_TEST_ENDED_MSG                      0x0B
#define         CAPITO_HELPER_MODEM_TEST_RESULT_MSG                         0x0C
#define         CAPITO_HELPER_LIMIT_UP_TEST_RESULT_MSG                      0x0D
#define         CAPITO_HELPER_LIMIT_DN_TEST_RESULT_MSG                      0x0E
#define         CAPITO_HELPER_EEPROM_TEST_RESULT_MSG                        0x0F
#define         CAPITO_HELPER_TARGET_ILLMINATOR_TEST_STARTED_MSG            0x10
#define         CAPITO_HELPER_TARGET_ILLMINATOR_TEST_ENDED_MSG              0x11
#define         CAPITO_HELPER_TARGET_MOVE_UP_DN_TEST_STARTED_MSG            0x12
#define         CAPITO_HELPER_TARGET_MOVE_UP_DN_TEST_ENDED_MSG              0x13
#define         CAPITO_HELPER_END_OF_TEST_ACK_MSG                           0xAB

/************************************************************************************/
#define			CAPITO_HELPER_MSG_SYNQ_SIZE_IN_BYTES				     	1
#define			CAPITO_HELPER_MSG_CHECKSUM_SIZE_IN_BYTES					1
#define			CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES					    0x0A
#define			CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES					    6
#define			CAPITO_HELPER_MSG_MSGTYPE_INDEX								2
#define 		CAPITO_HELPER_MSG_SYNQ_BYTE									0x55

/***********************************************************************************/

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum
{
	CAPITO_HELPER_SUCCESS,
	CAPITO_HELPER_ERROR_CHECKSUM,
	CAPITO_HELPER_INVALID_MSG_TYPE

}CapitoHelperStatus_et;


/***********************************************************************************/
// From PC TO CAPITO
typedef	struct
{
	uint8_t		ui8Synq;
	uint8_t		ui8Len;             //The whole message length
	uint8_t		ui8MsgType;
	uint8_t	     ui8Data[6];
	uint8_t		ui8CheckSum;       // Sum of all message bytes
}PCRStartMsg_st;


typedef	struct
{
	uint8_t		ui8Synq;
	uint8_t		ui8Len;       //The whole message length
	uint8_t		ui8MsgType;
	uint8_t	    ui8PCBID[4];     //4 bytes ID
	uint8_t     ui8Reserved[2];  //2 byte reserved
	uint8_t		ui8CheckSum;   // Sum of all message bytes
}PCSetPcbIDReqMsg_st;


/***********************************************************************************/
// From CAPITO TO PC

typedef	struct
{
	uint8_t		ui8Synq;
	uint8_t		ui8Len;         //The whole message length
	uint8_t		ui8MsgType;
	uint8_t	    ui8PCBID[4];     //4 bytes ID
	uint8_t     ui8Reserved[2];  //2 byte reserved
	uint8_t		ui8CheckSum;    // Sum of all message bytes
}CapitoTestStartedReplyMsg_st;

//pcb id , end of test
typedef	struct
{
	uint8_t		ui8Synq;
	uint8_t		ui8Len;            //The whole message length
	uint8_t		ui8MsgType;
	uint8_t	    ui8Reserved[6];   // 6 byte reserved
	uint8_t		ui8CheckSum;     // Sum of all message bytes
}CapitoSetAckMsg_st;

//battery , SPI DATA , HIT 1 & 2 , Extra Uart , Modem , limit switch , eeprom
typedef	struct
{
	uint8_t		ui8Synq;
	uint8_t		ui8Len;            //The whole message length
	uint8_t		ui8MsgType;
	uint8_t     ui8result;
	uint8_t	    ui8Reserved[5];   // 5 byte reserved
	uint8_t		ui8CheckSum;     // Sum of all message bytes
}CapitoResultMsg_st;



//started and ended clk test , CS12 , smoke box , Target Illiminator , Target move up and down ,
typedef	struct
{
	uint8_t		ui8Synq;
	uint8_t		ui8Len;            //The whole message length
	uint8_t		ui8MsgType;
	uint8_t	    ui8Reserved[6];   // 6 byte reserved
	uint8_t		ui8CheckSum;     // Sum of all message bytes
}CapitoStartedEndedTestMsg_st;


/*******************************************************************************
 *                            Extern Variables                            *
 *******************************************************************************/
extern uint32_t g_ui32PCBIdParsing;

/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/

CapitoHelperStatus_et	APP_CapitoHelper_CalculateCheckSum (void* 		        pCapitoMsg,
		uint8_t		    ui8PCapitoMsgSize,
		uint8_t*	        pui8CheckSum);

CapitoHelperStatus_et	APP_CapitoHelper_ParseMsg (void*     pCapitoRxMsg ,
		uint8_t	 ui8CapitoRxMsgSize,
		uint8_t   *pui8MsgType,
		uint32_t   *g_ui32PCBIdParsing );

CapitoHelperStatus_et	APP_PCHelper_ConstructMsg (void* 			pCapitoMsg ,
		uint8_t			ui8Msgtype,
		uint8_t         ui8Result,
		uint32_t        ui32PCBId);

#endif /* APP_APP_CAPITOHELPER_INC_APP_CAPITOHELPER_H_ */
