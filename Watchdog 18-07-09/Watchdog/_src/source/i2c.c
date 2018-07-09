/*
 * i2c.c
 *
 * Created: 6/27/2018 12:31:49 PM
 *  Author: braddock.bresnahan
 */ 

#include "i2c.h"
#include "driver_init.h"

/// Function Definitions

void InitI2C(){
	i2c_m_sync_get_io_descriptor(&I2C, &i2cMaster);
	i2c_m_sync_set_slaveaddr(&I2C, I2C_MOTION_ADDRESS, I2C_M_SEVEN);
	EnableI2C();
}

void ReadI2C(uint8* data, uint16 length){
	int timeout = 0;
	while (io_read(i2cMaster, data, length) == 0){
		if (timeout++ >= I2C_GENERAL_TIMEOUT)
		break;
	}
}

void WriteI2C(const uint8* data, uint16 length){
	int timeout = 0;
	while (!io_write(i2cMaster, data, length)){
		if (timeout++ >= I2C_GENERAL_TIMEOUT)
		break;
	}
}

void ReadReg(uint8 regAddr, uint8* data){
	int timeout = 0;
	while (i2c_m_sync_cmd_read(&I2C, regAddr, data, 1) == 0){
		if (timeout++ >= I2C_GENERAL_TIMEOUT)
		break;
	}
}

void WriteReg(uint8 regAddr, uint8 data){
	int timeout = 0;
	while (i2c_m_sync_cmd_write(&I2C, regAddr, &data, 1) == 0){
		if (timeout++ >= I2C_GENERAL_TIMEOUT)
		break;
	}
}

void SetReg(uint8 regAddr, uint8 data){
	static uint8 org;
	ReadReg(regAddr, &org);
	WriteReg(regAddr, org | data);
}

void ClearReg(uint8 regAddr, uint8 data){
	static uint8 org;
	ReadReg(regAddr, &org);
	WriteReg(regAddr, org & ~(data));
}