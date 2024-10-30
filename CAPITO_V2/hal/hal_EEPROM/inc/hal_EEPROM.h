/*
 * hal_EEPROM.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Rahma
 */

#ifndef HAL_HAL_EEPROM_HAL_EEPROM_H_
#define HAL_HAL_EEPROM_HAL_EEPROM_H_


/*******************************************************************************
*                               includes                             *
*******************************************************************************/


/*******************************************************************************
*                                Definitions                                  *
*******************************************************************************/
// Define the I2C address of the EEPROM
#define EEPROM_ADDR                    0x50
//#define EEPROM_ADDR                    0xA0
#define EEPROM_I2C1_PORT               0
#define EEPROM_I2C1_SDA_PIN            19
#define EEPROM_I2C1_SCL_PIN            20
#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000
//#define I2C_SLAVE_EEPROM_SIZE       64

/*******************************************************************************
*                               Types Declaration                             *
*******************************************************************************/


/*******************************************************************************
*                                Extern Variables                                 *
*******************************************************************************/
extern uint8_t g_aui8EEPROMTxBuffer[128];
extern uint8_t g_aui8EEPROMRxBuffer[128];

/*******************************************************************************
 *                             Function Declaration                             *
 *******************************************************************************/
void HAL_EEPROM_I2C1Pins_Configure(void);
void HAL_EEPROM_I2C1_Init();
uint8_t HAL_EEPROM_I2C1_WriteByte( uint8_t slaveAddr,uint16_t memoryAddr,uint8_t data);
void HAL_EEPROM_I2C1_WritePage( uint8_t slaveAddr, uint16_t memoryAddr, const uint8_t *data, uint8_t length);
void HAL_EEPROM_I2C1_ReadByte( uint8_t slaveAddr, uint16_t memoryAddr);
void HAL_EEPROM_I2C1_ReadPage( uint8_t slaveAddr, uint16_t memoryAddr, uint8_t *data, uint8_t length);

#endif /* HAL_HAL_EEPROM_HAL_EEPROM_H_ */
