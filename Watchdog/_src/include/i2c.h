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

#define I2C_MOTION_ADDRESS				0x69 // Can be set to 0x68 or 0x69, see BMI160 doc

#define I2C_GENERAL_TIMEOUT	250 // In milliseconds
#define I2C_INIT_RETRIES	5 // Number of connection retries before it gives up

#define I2C_BUMP_DELAY		10 // In milliseconds. Used for debugging
#define I2C_RESPONSE_STRING		"_--------_" // Debugging
#define I2C_NO_RESPONSE_STRING	"_------------_" // Debugging

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
// Desc: Reads the given register from the BMI160 and stores the value in data.
// Input: (uint8_t)    addr: Dummy variable. Has no use.
//        (uint8_t)     reg: The register to read from.
//        (uint8_t*)   data: The container to store the read value.
//        (uint16_t) length: The number of bytes to read.
// Return: (uint8_t) 0.
*/
int8_t ReadI2C(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t length);
/*
// Name: WriteI2C
// Desc: Writes the given data to the given register in the BMI160.
// Input: (uint8_t)    addr: Dummy variable. Has no use.
//        (uint8_t)     reg: The register to write to.
//        (uint8_t*)   data: The value(s) to write.
//        (uint16_t) length: The number of values from data to write.
*/
int8_t WriteI2C(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t length);

/*
// Name: i2cMaster
// Desc: The master descriptor for I2C.
*/
struct io_descriptor* i2cMaster;


#endif /* I2C_H_ */