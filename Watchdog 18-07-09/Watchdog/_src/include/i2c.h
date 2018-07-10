/*
 * i2c.h
 *
 * Created: 6/27/2018 12:25:17 PM
 *  Author: braddock.bresnahan
 */ 


#ifndef I2C_H_
#define I2C_H_

#include "stdtypes.h"
#include "driver_init.h"

/// Macros

// Board Specific Information
#define I2C_MASTER_MODULE				SERCOM1
#define I2C_MASTER_PINMUX_PAD0			EXT1_I2C_SERCOM_PINMUX_PAD0;
#define I2C_MASTER_PINMUX_PAD1			EXT1_I2C_SERCOM_PINMUX_PAD1;
#define I2C_GENERAL_TIMEOUT				1000
#define I2C_MOTION_ADDRESS				0x68 // Can be set to 0x69, see doc
#define I2C_MASTER_ENABLE_ON_INIT		true

// Accelerometer Specific Information/Settings
#define I2C_ACC_CONF_REG					0x40
#define I2C_ACC_CONF_US						(1 << 7) // Undersampling
#define I2C_ACC_CONF_BWP					(2 << 4) // Averaging cycles
#define I2C_ACC_CONF_ODR					(6 << 0) // ODR 25
#define I2C_ACC_RANGE_REG					0x41
#define I2C_ACC_RANGE						3 // +- 2g range
#define I2C_ACC_INT_EN0_REG					0x50
#define I2C_ACC_INT_EN1_REG					0x51
#define I2C_ACC_INT_EN2_REG					0x52
#define I2C_ACC_INT_EN0						0b00000111 // Enable any-motion x, y and z
#define I2C_ACC_INT_EN1						0b00000000 // Keep all these off
#define I2C_ACC_INT_EN2						0b00000111 // Enable no-motion interrupts
#define I2C_ACC_INT_OUT_CTRL_REG			0x53
#define I2C_ACC_INT_OUT_CTRL				0b00001111 // Enable interrupt 1
#define I2C_ACC_INT_MAP0_REG				0x55
#define I2C_ACC_INT_MAP1_REG				0x56
#define I2C_ACC_INT_MAP2_REG				0x57
#define I2C_ACC_INT_MAP0					0b11111111 // Connect interrupts to int 1
#define I2C_ACC_INT_MAP1					0b11110000 // Connect interrupts to int 1, not int 2
#define I2C_ACC_INT_MAP2					0b00000000 // Don't connect to int 2
#define I2C_ACC_INT_MOTION0_REG				0x5F
#define I2C_ACC_INT_MOTION1_REG				0x60
#define I2C_ACC_INT_MOTION2_REG				0x61
#define I2C_ACC_INT_MOTION3_REG				0x62
#define I2C_ACC_INT_MOTION0_SLONO_DUR		(0 << 2)
#define I2C_ACC_INT_MOTION0_ANYM_DUR		(0 << 0)
#define I2C_ACC_INT_MOTION1					0b00010100
#define I2C_ACC_INT_MOTION2					0b00010100
#define I2C_ACC_INT_MOTION3_SIG_MOT_PROOF	(0 << 4)
#define I2C_ACC_INT_MOTION3_SIG_MOT_SKIP	(0 << 2)
#define I2C_ACC_INT_MOTION3_SIG_MOT_SEL		(1 << 1)
#define I2C_ACC_INT_MOTION3_NO_MOT_SEL		(0 << 0)

#define I2C_ACC_CMD						0x7E
#define I2C_ACC_CMD_ACC_POWER_MODE		0b00010000
#define I2C_ACC_CMD_GYR_POWER_MODE		0b00010100
#define I2C_ACC_CMD_MAG_POWER_MODE		0b00011000
#define I2C_ACC_CMD_NORMAL_POWER		0b01
#define I2C_ACC_CMD_LOWPOWER			0b10
#define I2C_ACC_CMD_SUSPEND				0b00
#define I2C_ACC_CMD_FAST_STARTUP		0b11
#define I2C_ACC_CMD_ACC_WAIT_TIME		5
#define I2C_ACC_CMD_GYR_WAIT_TIME		81
#define I2C_ACC_CMD_MAG_WAIT_TIME		1

/// Function Declarations

/*
// Name: InitI2C
// Desc: Initializes the I2C interface and configures the accelerometer.
*/
void InitI2C(void);

/*
// Name: EnableI2C
// Desc: Enables I2C communication.
*/
inline void EnableI2C(void) {
	i2c_m_sync_enable(&I2C);
}
/*
// Name: DisableI2C
// Desc: Disables I2C communication.
*/
inline void DisableI2C(void){
	i2c_m_sync_disable(&I2C);
}

/*
// Name: ReadI2C
// Desc: Reads the number of bytes given by length into data.
// Input: (uint8*)   data: The container to store the read data.
//        (uint16) length: The number of bytes to read.
*/
void ReadI2C(uint8* data, uint16 length);
/*
// Name: WriteI2C
// Desc: Writes the number of bytes given by length to the peripheral from data.
// Input: (const uint8*) data: The data to write.
//        (uint16) length: The number of bytes to write.
*/
void WriteI2C(const uint8* data, uint16 length);

/*
// Name: ReadReg
// Desc: Reads the register from the given address into the given data using I2C.
// Input: (uint8) regAddr: The register address to read from.
//        (uint8*) data: The container to store the read register.
*/
void ReadReg(uint8 regAddr, uint8* data);
/*
// Name: ReadRegs
// Desc: Reads multiple registers starting from the given address into the given data
//       using I2C.
// Input: (uint) startRegAddr: The starting register's address.
//        (uint8) numberOfBytes: The number of bytes/registers to read.
//        (uint8*) data: The container to read into.
*/
void ReadRegs(uint startRegAddr, uint8 numberOfBytes, uint8* data);
/*
// Name: WriteReg
// Desc: Writes the given data byte to the given register address using I2C.
// Input: (uint8) regAddr: The register to write to.
//        (uint8) data: The data to write.
*/
void WriteReg(uint8 regAddr, uint8 data);
/*
// Name: SetReg
// Desc: Sets the bits in the given register address with the given mask using I2C.
// Input: (uint8) regAddr: The register to set the bits of.
//        (uint8)    mask: The bits to set.
*/
void SetReg(uint8 regAddr, uint8 mask);
/*
// Name: ClearReg
// Desc: Clears the bits in the given register address with the given mask using I2C.
// Input: (uint8) regAddr: The register to clear the bits of.
//        (uint8)    mask: The bits to clear.
*/
void ClearReg(uint8 regAddr, uint8 mask);

/// Global Variables

/*
// Name: i2cMaster
// Desc: The master object for handling I2C stuff.
*/
struct io_descriptor* i2cMaster;


#endif /* I2C_H_ */