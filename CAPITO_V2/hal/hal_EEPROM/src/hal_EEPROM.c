/*
 * hal_EEPROM.c
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
#include "chip_lpc175x_6x.h"
#include "i2c_17xx_40xx.h"
#include "hal_EEPROM.h"
#include "string.h"
/*******************************************************************************
 *                               Global Variables                             *
 *******************************************************************************/
uint8_t g_ui8senddatasize=0;
uint8_t g_ui8readdatasize=0;
uint8_t g_aui8EEPROMTxBuffer[128]={0};
uint8_t g_aui8EEPROMRxBuffer[128]={0};
I2C_XFER_T xfer;
/*******************************************************************************
 *                              Function Definition                            *
 *******************************************************************************/
void HAL_EEPROM_I2C1Pins_Configure(void)
{
	//	// Configure I2C1 SDA pin (P0.19)
	//	Chip_IOCON_PinMux(LPC_IOCON, EEPROM_I2C1_PORT, EEPROM_I2C1_SDA_PIN, IOCON_MODE_INACT, IOCON_FUNC3);
	//
	//	// Configure I2C1 SCL pin (P0.20)
	//	Chip_IOCON_PinMux(LPC_IOCON, EEPROM_I2C1_PORT, EEPROM_I2C1_SCL_PIN, IOCON_MODE_INACT, IOCON_FUNC3);

	// Configure I2C1 SDA pin (P0.19)
	Chip_IOCON_PinMuxSet(LPC_IOCON, EEPROM_I2C1_PORT, EEPROM_I2C1_SDA_PIN, IOCON_MODE_INACT| IOCON_FUNC3);

	// Configure I2C1 SCL pin (P0.20)
	Chip_IOCON_PinMuxSet(LPC_IOCON, EEPROM_I2C1_PORT, EEPROM_I2C1_SCL_PIN, IOCON_MODE_INACT| IOCON_FUNC3);

	Chip_IOCON_EnableOD(LPC_IOCON, EEPROM_I2C1_PORT, EEPROM_I2C1_SDA_PIN);
	Chip_IOCON_EnableOD(LPC_IOCON, EEPROM_I2C1_PORT, EEPROM_I2C1_SCL_PIN);

}

void HAL_EEPROM_I2C1_Init()
{
	HAL_EEPROM_I2C1Pins_Configure();
	//	Board_I2C_Init(I2C1);
	Chip_I2C_Init(I2C1);
	Chip_I2C_SetClockRate(I2C1, SPEED_100KHZ);
	NVIC_DisableIRQ(I2C1_IRQn);
	Chip_I2C_SetMasterEventHandler(I2C1, Chip_I2C_EventHandlerPolling);
}

uint8_t HAL_EEPROM_I2C1_WriteByte( uint8_t slaveAddr, uint16_t memoryAddr ,uint8_t data)
{
	uint8_t ui8DataWrite=0;
	memset ((void *) g_aui8EEPROMTxBuffer , 0 , 10);
	memset ((void *) g_aui8EEPROMRxBuffer , 0 , 10);
	g_aui8EEPROMTxBuffer[0] = (uint8_t)(memoryAddr >> 8); // MSB of memory address
	g_aui8EEPROMTxBuffer[1] = (uint8_t)(memoryAddr & 0xFF); // LSB of memory address
	g_aui8EEPROMTxBuffer[2] = data; // Data to be written

	ui8DataWrite=g_aui8EEPROMTxBuffer[2];

	xfer.rxBuff = g_aui8EEPROMRxBuffer;
	xfer.txBuff = g_aui8EEPROMTxBuffer;
	xfer.slaveAddr = slaveAddr;
	xfer.rxSz = 0;
	xfer.txSz = 3;
	g_ui8senddatasize = Chip_I2C_MasterTransfer(I2C1, &xfer);

	return ui8DataWrite;

}

void HAL_EEPROM_I2C1_WritePage( uint8_t slaveAddr, uint16_t memoryAddr, const uint8_t *data, uint8_t length)
{
	memset ((void *) g_aui8EEPROMTxBuffer , 0 , 128);
	memset ((void *) g_aui8EEPROMRxBuffer , 0 , 128);

	//	uint8_t buffer[length + 2];
	//	g_aui8EEPROMTxBuffer[0] = (uint8_t)(memoryAddr >> 8); // MSB of memory address
	//	g_aui8EEPROMTxBuffer[1] = (uint8_t)(memoryAddr & 0xFF); // LSB of memory address

	g_aui8EEPROMTxBuffer[0] = (uint8_t)((memoryAddr >> 7) & 0xFF);  // Page number
	g_aui8EEPROMTxBuffer[1] = (uint8_t)(memoryAddr & 0x7F);         // Byte index within the page

	for (uint8_t i = 0; i < length; i++) {
		g_aui8EEPROMTxBuffer[i + 2] = data[i];
	}
	xfer.rxBuff = g_aui8EEPROMRxBuffer;
	xfer.txBuff = g_aui8EEPROMTxBuffer;
	xfer.slaveAddr = slaveAddr;
	xfer.rxSz = 0;
	xfer.txSz = length+2;

	g_ui8senddatasize=Chip_I2C_MasterTransfer(I2C1, &xfer);
}

void HAL_EEPROM_I2C1_ReadByte( uint8_t slaveAddr, uint16_t memoryAddr)
{

	// Read Data @ 0
	memset ((void *) g_aui8EEPROMTxBuffer , 0 , 10);
	memset ((void *) g_aui8EEPROMRxBuffer , 0 , 10);
	g_aui8EEPROMTxBuffer[0] = (uint8_t)(memoryAddr >> 8); // MSB of memory address
	g_aui8EEPROMTxBuffer[1] = (uint8_t)(memoryAddr & 0xFF); // LSB of memory address

	xfer.slaveAddr = slaveAddr ;
	xfer.txBuff = g_aui8EEPROMTxBuffer;
	xfer.txSz = 2;
	xfer.rxBuff = g_aui8EEPROMRxBuffer;
	xfer.rxSz = 1;
	g_ui8senddatasize = Chip_I2C_MasterTransfer(I2C1, &xfer);




}
void HAL_EEPROM_I2C1_ReadPage(uint8_t slaveAddr, uint16_t memoryAddr, uint8_t *data, uint8_t length)
{
	// Read Data @ 0
	memset ((void *) g_aui8EEPROMTxBuffer , 0 , 128);
	memset ((void *) g_aui8EEPROMRxBuffer , 0 , 128);
	//	g_aui8EEPROMTxBuffer[0] = (uint8_t)(memoryAddr >> 8); // MSB of memory address
	//	g_aui8EEPROMTxBuffer[1] = (uint8_t)(memoryAddr & 0xFF); // LSB of memory address

	g_aui8EEPROMTxBuffer[0] = (uint8_t)((memoryAddr >> 7) & 0xFF);  // Page number
	g_aui8EEPROMTxBuffer[1] = (uint8_t)(memoryAddr & 0x7F);         // Byte index within the page

	xfer.slaveAddr = slaveAddr;
	xfer.txBuff = g_aui8EEPROMTxBuffer;
	xfer.txSz = 2;
	xfer.rxBuff = g_aui8EEPROMRxBuffer;
	xfer.rxSz = length;

	g_ui8readdatasize = Chip_I2C_MasterTransfer(I2C1, &xfer);
	for (uint8_t i = 0; i < length; i++) {
		data[i] = g_aui8EEPROMRxBuffer[i];
		}
}
