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
		MotorEnqueue("_--__--_");
	}
	else{
		MotorEnqueue("_-_-_");
		LEDEnqueue(LED, "+=+=+");
	}
		
}



