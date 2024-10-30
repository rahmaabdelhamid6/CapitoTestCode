/*
 * app_CapitoHelper.c
 *
 *  Created on: Jan 8, 2024
 *      Author: Rahma
 */

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "lpc_types.h"
#include "board.h"
#include "LPC17xx.h"
#include "string.h"
#include "app_CapitoHelper.h"


/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/

uint8_t  g_aui8PCtoCapitoData[CAPITO_HELPER_MSG_LENGTH_SIZE_IN_BYTES] ;

uint32_t g_ui32PCBIdParsing = 0;
/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/

CapitoHelperStatus_et	APP_CapitoHelper_CalculateCheckSum (void* 		        pCapitoMsg,
		uint8_t		    ui8PCapitoMsgSize,
		uint8_t*	        pui8CheckSum)
{
	CapitoHelperStatus_et		etCapitoHelperStatus = CAPITO_HELPER_SUCCESS;

	uint8_t					*pui8Buffer = (uint8_t*)pCapitoMsg;

	uint8_t					ui8CheckSum = 0;
	uint32_t				ui32Index;


	for (ui32Index = 0 ; ui32Index < ui8PCapitoMsgSize-1 ; ui32Index++)
	{
		ui8CheckSum += pui8Buffer[ui32Index];
	}


	*pui8CheckSum = ui8CheckSum;


	return etCapitoHelperStatus;

}

CapitoHelperStatus_et	APP_CapitoHelper_ParseMsg (void*     pCapitoRxMsg ,
		uint8_t	 ui8CapitoRxMsgSize,
		uint8_t   *pui8MsgType,
		uint32_t   *g_ui32PCBIdParsing )
{

	CapitoHelperStatus_et   etCapitoHelperStatus = CAPITO_HELPER_SUCCESS;
	uint8_t				ui8CalculatedCheckSum = 0;
	uint8_t				ui8CapitoRxCheckSum = 0;
	uint32_t                *paui32PCBID;

	PCRStartMsg_st *pstPCRStartMsg ;
	PCSetPcbIDReqMsg_st  *pstPCSetPcbIDReqMsg;

	APP_CapitoHelper_CalculateCheckSum(pCapitoRxMsg, ui8CapitoRxMsgSize, &ui8CalculatedCheckSum);

	ui8CapitoRxCheckSum = ((uint8_t*)pCapitoRxMsg)[ui8CapitoRxMsgSize - 1] ;

	// Verify Check Sum
	if (ui8CapitoRxCheckSum != ui8CalculatedCheckSum )
	{
		return CAPITO_HELPER_ERROR_CHECKSUM;
	}

	switch (((uint8_t *)pCapitoRxMsg)[CAPITO_HELPER_MSG_MSGTYPE_INDEX])
	{
	case CAPITO_HELPER_START_TEST_REQ_MSG:

		pstPCRStartMsg = ((PCRStartMsg_st*) pCapitoRxMsg);
		*pui8MsgType =  pstPCRStartMsg->ui8MsgType;

//		memset(g_aui8PCtoCapitoData,0,CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
//		memcpy(g_aui8PCtoCapitoData , (void *)&pstPCRStartMsg->ui8Data , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);

		break ;


	case CAPITO_HELPER_SET_PCB_ID_REQ_MSG:

		pstPCSetPcbIDReqMsg = ((PCSetPcbIDReqMsg_st*) pCapitoRxMsg);
		*pui8MsgType =  pstPCSetPcbIDReqMsg->ui8MsgType;
		paui32PCBID=(uint32_t *)pstPCSetPcbIDReqMsg->ui8PCBID;
		*g_ui32PCBIdParsing = *paui32PCBID;
		memset((void *)&pstPCSetPcbIDReqMsg->ui8Reserved , 0 , 2);

//		memset(g_aui8PCtoCapitoData,0,CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
//		memcpy(g_aui8PCtoCapitoData , (void *)&pstPCSetPcbIDReqMsg->ui8Reserved , 2);
		break ;
	}

	return etCapitoHelperStatus;

}

CapitoHelperStatus_et	APP_PCHelper_ConstructMsg (void*   pCapitoMsg ,
		uint8_t			ui8Msgtype,
		uint8_t         ui8Result,
		uint32_t        ui32PCBId)
{

	CapitoHelperStatus_et etCapitoHelperStatus = CAPITO_HELPER_SUCCESS;
	uint8_t				    ui8CheckSum = 0;
	uint8_t					ui8MsgLen = 0;
	uint32_t                *paui32PCBID;

	CapitoTestStartedReplyMsg_st *pstCapitoTestStartedReplyMsg;
	CapitoSetAckMsg_st           *pstCapitoSetAckMsg;
	CapitoResultMsg_st           *pstCapitoResultMsg;
	CapitoStartedEndedTestMsg_st *pstCapitoStartedEndedTestMsg;

	// Check Msg Type
	switch (ui8Msgtype)
	{
	case  CAPITO_HELPER_TEST_STARTED_REPLY_MSG:

		pstCapitoTestStartedReplyMsg = (CapitoTestStartedReplyMsg_st *)pCapitoMsg;
		// Assign Msg Len
		ui8MsgLen =  sizeof (CapitoTestStartedReplyMsg_st);
		// Reset Capito Test Started Reply Msg Buffer
		memset (pstCapitoTestStartedReplyMsg , 0 , sizeof (CapitoTestStartedReplyMsg_st));
		pstCapitoTestStartedReplyMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoTestStartedReplyMsg->ui8Len =  sizeof (CapitoTestStartedReplyMsg_st);
		pstCapitoTestStartedReplyMsg->ui8MsgType = ui8Msgtype;

		//pcb id lsa hayt3ml
		paui32PCBID=(uint32_t *)pstCapitoTestStartedReplyMsg->ui8PCBID;

		*paui32PCBID= ui32PCBId;

		memset((void *)&pstCapitoTestStartedReplyMsg->ui8Reserved , 0 , 2);

		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoTestStartedReplyMsg->ui8CheckSum = ui8CheckSum;


		break;
	case  CAPITO_HELPER_SET_PCB_ID_ACK_MSG:

		ui8MsgLen =  sizeof (CapitoSetAckMsg_st);
		pstCapitoSetAckMsg = (CapitoSetAckMsg_st *)pCapitoMsg;
		memset (pstCapitoSetAckMsg , 0 , sizeof (CapitoSetAckMsg_st));
		pstCapitoSetAckMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoSetAckMsg->ui8Len =  sizeof (CapitoSetAckMsg_st);
		pstCapitoSetAckMsg->ui8MsgType = ui8Msgtype;

		memset((void *)&pstCapitoSetAckMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoSetAckMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_BATTERY_LEVEL_TEST_RESULT_MSG:

		ui8MsgLen =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SPI_CLK_TEST_STARTED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SPI_CLK_TEST_ENDED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SPI_DATA_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SPI_CS1_CS2_TEST_STARTED_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SPI_CS1_CS2_TEST_ENDED_RESULT_MSG:

		ui8MsgLen =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SPI_HIT1_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SPI_HIT2_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_EXTRA_UART_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SMOKE_BOX_TEST_STARTED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_SMOKE_BOX_TEST_ENDED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_MODEM_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_LIMIT_UP_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_LIMIT_DN_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;

	case  CAPITO_HELPER_EEPROM_TEST_RESULT_MSG:

		ui8MsgLen = sizeof(CapitoResultMsg_st);
		pstCapitoResultMsg = (CapitoResultMsg_st *)pCapitoMsg;
		memset (pstCapitoResultMsg , 0 , sizeof (CapitoResultMsg_st));
		pstCapitoResultMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoResultMsg->ui8Len =  sizeof (CapitoResultMsg_st);
		pstCapitoResultMsg->ui8MsgType = ui8Msgtype;
		pstCapitoResultMsg->ui8result = ui8Result ;
		memset((void *)&pstCapitoResultMsg->ui8Reserved , 0 , (CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES-1));
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoResultMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_TARGET_ILLMINATOR_TEST_STARTED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;


		break;
	case  CAPITO_HELPER_TARGET_ILLMINATOR_TEST_ENDED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;


		break;
	case  CAPITO_HELPER_TARGET_MOVE_UP_DN_TEST_STARTED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_TARGET_MOVE_UP_DN_TEST_ENDED_MSG:

		ui8MsgLen = sizeof(CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg = (CapitoStartedEndedTestMsg_st *)pCapitoMsg;
		memset (pstCapitoStartedEndedTestMsg , 0 , sizeof (CapitoStartedEndedTestMsg_st));
		pstCapitoStartedEndedTestMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoStartedEndedTestMsg->ui8Len =  sizeof (CapitoStartedEndedTestMsg_st);
		pstCapitoStartedEndedTestMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoStartedEndedTestMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoStartedEndedTestMsg->ui8CheckSum = ui8CheckSum;

		break;
	case  CAPITO_HELPER_END_OF_TEST_ACK_MSG:

		ui8MsgLen = sizeof(CapitoSetAckMsg_st);
		pstCapitoSetAckMsg = (CapitoSetAckMsg_st *)pCapitoMsg;
		memset (pstCapitoSetAckMsg , 0 , sizeof (CapitoSetAckMsg_st));
		pstCapitoSetAckMsg->ui8Synq = CAPITO_HELPER_MSG_SYNQ_BYTE;
		pstCapitoSetAckMsg->ui8Len =  sizeof (CapitoSetAckMsg_st);
		pstCapitoSetAckMsg->ui8MsgType = ui8Msgtype;
		memset((void *)&pstCapitoSetAckMsg->ui8Reserved , 0 , CAPITO_HELPER_DATA_LENGTH_SIZE_IN_BYTES);
		APP_CapitoHelper_CalculateCheckSum(pCapitoMsg, ui8MsgLen, &ui8CheckSum);
		pstCapitoSetAckMsg->ui8CheckSum = ui8CheckSum;

		break;
	}

	return etCapitoHelperStatus;
}
