//

#include <stdlib.h>
#include "atmel_start.h"
#include "hpl_sleep.h"
#include "queue.h"
#include "array.h"

#include "stdtypes.h"
#include "boardfuncs.h"
#include "band.h"
#include "modes.h"
#include "messages.h"
#include "timers.h"
#include "interrupts.h"
#include "i2c.h"

#include "power.h"
#include "motor.h"

/// Function Declarations

// Main
int main(void);
void Init(void);

/* NOTE: It might matter if the vibrator sets off the accelerometer. You should turn off or reduce the sensitivity of the accelerometer while the device in vibrating */ // Or only worry about 

/// Main

int main(void)
{
	Init();
	
	while (1) BandMain();
	
	return 0;
}

void Init(){
	MCUBusy = 0;
	MCUSkip = false;
	atmel_start_init();
	InitPowerModule();
	InitEvents();
	InitTimers();
	InitInterrupts();
	InitI2C();
	
	if (false){
		MotorEnqueue("_-_-_");
		MotorDelay(1000);
		MotorEnqueue("_-_-___----_____-_-___----_");
		MotorDelay(1000);
		MotorEnqueue("_--__--_____--------______--__--_____--------_");
		MotorDelay(1000);
		for (int i = 0; i < 16; i++){
			MotorEnqueue("-_");
		}
		MotorDelay(1000);
		MotorEnqueue("_------------------------------_");
	}
	
	/// Setting up accelerometer
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
	
	/// Setting up BlueTooth
	
	/// Other
	isHost = false;
	connected = false;
	bandRange = STARTING_BANDRANGE;
	button = false;
	led = LEDW;
	
	EnableTimer(&TIMER0);
	InitNormal();
	//LEDDelay(1000);
	
	// Successful Init Indicator
	static const bool testing = false;
	if (testing){
		MotorEnqueue("_-_-_");
	}
	else{
		MotorEnqueue("_-_-_");
		LEDEnqueue(LED, "+=+=+");
	}
		
}



