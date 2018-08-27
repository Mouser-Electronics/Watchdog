/*
 * i2c.c
 *
 * Created: 6/27/2018 12:31:49 PM
 *  Author: braddock.bresnahan
 */ 

#include "i2c.h"
#include <stdlib.h>
#include <string.h>
#include "bmi160.h"
#include "led.h"

/// "Private" Functions

/*
// Name: I2CReportSuccess
// Desc: Blinks the LED specific colors depending on the given input.
// Input: (bool) command: The success status of the command.
// Return: (bool) The given command status.
*/
bool I2CReportSuccess(bool command){
	uint8 led = LEDG;
	
	while (!LEDIsIdle());
	if (command != BMI160_OK) led = LEDR;
	SetLED(led, ON);
	delay_ms(I2C_BUMP_DELAY);
	SetLED(led, OFF);
	
	return command;
}

/*
// Name: delayI2C
// Desc: A mask for the delay function so it can work with I2C driver.
// Input: (uint32_t) period: The period of time to delay in milliseconds.
*/
void delayI2C(uint32_t period){
	delay_ms((uint16_t)period);
}

/// Function Definitions

void InitI2C(){
	uint timeout = 0;
	struct bmi160_dev bmi;
	struct bmi160_int_settg bmicfg;
	
	i2cMaster = NULL;
	
	// Setup board I2C
	i2c_m_sync_get_io_descriptor(&I2C, &i2cMaster);
	i2c_m_sync_set_slaveaddr(&I2C, I2C_MOTION_ADDRESS, I2C_M_SEVEN);
	EnableI2C();
	
	// Configure I2C Driver
	bmi.id = I2C_MOTION_ADDRESS;		// The I2C address of the bmi160
	bmi.interface = BMI160_I2C_INTF;	// Choose I2C interface
	bmi.read = ReadI2C;					// Set read function over I2C
	bmi.write = WriteI2C;				// Set write function over I2C
	bmi.delay_ms = delayI2C;			// Generic delay ms

	// Config BMI160
	while (I2CReportSuccess(bmi160_init(&bmi)) != BMI160_OK){ // Check for responsiveness
		if (timeout++ < I2C_INIT_RETRIES){
			delay_ms(I2C_GENERAL_TIMEOUT);
		}
		else{ // Can't connect
			LEDEnqueue(LEDR, I2C_NO_RESPONSE_STRING);
			return;
		}
	}
	LEDEnqueue(LEDG, I2C_RESPONSE_STRING);
	
	bmi.accel_cfg.power = BMI160_ACCEL_LOWPOWER_MODE; // Set to low power mode

	I2CReportSuccess(bmi160_set_power_mode(&bmi)); // Apply setting

	bmicfg.int_channel = BMI160_INT_CHANNEL_1; // Interrupt 1 pin

	bmicfg.int_type = BMI160_ACC_ANY_MOTION_INT ;					// Any-motion interrupt
	bmicfg.int_pin_settg.output_en = BMI160_ENABLE;					// Set as output interrupt
	bmicfg.int_pin_settg.output_mode = BMI160_DISABLE;				// Pull for interrupt pin
	bmicfg.int_pin_settg.output_type = BMI160_ENABLE;				// Active high output
	bmicfg.int_pin_settg.edge_ctrl = BMI160_ENABLE;					// Continuous stream output
	bmicfg.int_pin_settg.input_en = BMI160_DISABLE;					// Disable input
	bmicfg.int_pin_settg.latch_dur = BMI160_LATCH_DUR_5_MILLI_SEC;	// Slightly latched

	bmicfg.int_type_cfg.acc_any_motion_int.anymotion_en = BMI160_ENABLE;	// Enable the interrupt
	bmicfg.int_type_cfg.acc_any_motion_int.anymotion_x = BMI160_ENABLE;		// Enable x-axis for interrupt
	bmicfg.int_type_cfg.acc_any_motion_int.anymotion_y = BMI160_ENABLE;		// Enable y-axis for interrupt
	bmicfg.int_type_cfg.acc_any_motion_int.anymotion_z = BMI160_ENABLE;		// Enable z-axis for interrupt
	bmicfg.int_type_cfg.acc_any_motion_int.anymotion_dur = 3;				// Max duration
	bmicfg.int_type_cfg.acc_any_motion_int.anymotion_thr = 7;				// Lower number = Higher sensitivity (but more false positives)

	I2CReportSuccess(bmi160_set_int_config(&bmicfg, &bmi)); // Apply settings
	
	DisableI2C(); // Done with configuring and I2C
}

int8_t ReadI2C(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t length){
	int timeout = 0;
	
	SetLED(LEDD, ON);
	while (i2c_m_sync_cmd_read(&I2C, reg, data, length) != 0){
		if (timeout++ >= I2C_GENERAL_TIMEOUT)
		break;
	}
	
	SetLED(LEDD, OFF);
	return 0;
}

int8_t WriteI2C(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t length){
	int timeout = 0;
	uint8* message = malloc(length + 1);
	
	SetLED(LEDD, ON);	
	message[0] = reg;
	memcpy((message + 1), data, length);
	while (io_write(i2cMaster, message, length + 1) != 0){
		if (timeout++ >= I2C_GENERAL_TIMEOUT)
		break;
	}
	free(message);
	
	SetLED(LEDD, OFF);
	return 0;
}

