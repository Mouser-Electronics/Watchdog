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
	i2cMaster = NULL;
	i2c_m_sync_get_io_descriptor(&I2C, &i2cMaster);
	i2c_m_sync_set_slaveaddr(&I2C, I2C_MOTION_ADDRESS, I2C_M_SEVEN);
	EnableI2C();
	
	// Config Accelerometer
	if (i2cMaster != NULL){ // Make sure it's initialized
		WriteReg(I2C_ACC_CONF_REG, I2C_ACC_CONF_US | I2C_ACC_CONF_BWP | I2C_ACC_CONF_ODR);
		
		WriteReg(I2C_ACC_RANGE_REG, I2C_ACC_RANGE);
		// Config Interrupts
		WriteReg(I2C_ACC_INT_EN0_REG, I2C_ACC_INT_EN0);
		WriteReg(I2C_ACC_INT_EN1_REG, I2C_ACC_INT_EN1);
		WriteReg(I2C_ACC_INT_EN2_REG, I2C_ACC_INT_EN2);
		
		WriteReg(I2C_ACC_INT_OUT_CTRL_REG, I2C_ACC_INT_OUT_CTRL);

		WriteReg(I2C_ACC_INT_MAP0_REG, I2C_ACC_INT_MAP0);
		WriteReg(I2C_ACC_INT_MAP1_REG, I2C_ACC_INT_MAP1);
		WriteReg(I2C_ACC_INT_MAP2_REG, I2C_ACC_INT_MAP2);

		WriteReg(I2C_ACC_INT_MOTION0_REG, I2C_ACC_INT_MOTION0_SLONO_DUR | I2C_ACC_INT_MOTION0_ANYM_DUR);
		WriteReg(I2C_ACC_INT_MOTION1_REG, I2C_ACC_INT_MOTION1);
		WriteReg(I2C_ACC_INT_MOTION2_REG, I2C_ACC_INT_MOTION2);
		WriteReg(I2C_ACC_INT_MOTION3_REG, I2C_ACC_INT_MOTION3_SIG_MOT_PROOF | I2C_ACC_INT_MOTION3_SIG_MOT_SKIP | I2C_ACC_INT_MOTION3_SIG_MOT_SEL | I2C_ACC_INT_MOTION3_NO_MOT_SEL);

		// Set Power modes
		WriteReg(I2C_ACC_CMD, I2C_ACC_CMD_ACC_POWER_MODE | I2C_ACC_CMD_LOWPOWER);
		delay_ms(I2C_ACC_CMD_ACC_WAIT_TIME);
		
		WriteReg(I2C_ACC_CMD, I2C_ACC_CMD_GYR_POWER_MODE | I2C_ACC_CMD_SUSPEND);
		delay_ms(I2C_ACC_CMD_GYR_WAIT_TIME);
		
		WriteReg(I2C_ACC_CMD, I2C_ACC_CMD_MAG_POWER_MODE | I2C_ACC_CMD_SUSPEND);
		delay_ms(I2C_ACC_CMD_MAG_WAIT_TIME);
	}
	
	DisableI2C(); // Wont be needing I2C anymore.
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

void SetReg(uint8 regAddr, uint8 mask){
	static uint8 org;
	ReadReg(regAddr, &org);
	WriteReg(regAddr, org | mask);
}

void ClearReg(uint8 regAddr, uint8 mask){
	static uint8 org;
	ReadReg(regAddr, &org);
	WriteReg(regAddr, org & ~(mask));
}