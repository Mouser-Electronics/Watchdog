/*
 * power.h
 *
 * Created: 6/27/2018 5:26:13 PM
 *  Author: braddock.bresnahan
 */ 


#ifndef POWER_H_
#define POWER_H_

#include "hpl_sleep.h"
#include "hpl_delay.h"
#include "stdtypes.h"

/// Macros

#ifndef PM
#define PM						0x40000400 // The address of the power manager
#endif
#define SLEEPMODE_IDLE			0x2
#define SLEEPMODE_STANDBY		0x4
#define VREGSMOD_AUTO			(0x0 << 6)
#define VREGSMOD_PERFORMANCE	(0x1 << 6)
#define VREGSMOD_LP				(0x2 << 6)
#define BBIASHS_BACK_BIAS		(1 << 10)

/// Enums

/*
// Name: PowerMode
// Desc: The possible sleep states of the MCU.
*/
typedef enum{
	POWERMODE_IDLE		= SLEEPMODE_IDLE,		// Soft sleep. Low power consumption. Short wakeup time.
	POWERMODE_STANDBY	= SLEEPMODE_STANDBY,	// Hard sleep. Lowest power consumption. Long wakeup time.
} PowerMode;

/// Functions

/*
// Name: SetPowerMode
// Desc: Sets up the power mode to use next time the board is put to sleep.
// Input: (PowerMode) mode: The power mode to use for sleep.
*/
inline static void SetPowerMode(PowerMode mode){
	hri_pm_set_SLEEPCFG_SLEEPMODE_bf((void*)PM, (uint8)mode);
	delay_ms(2); // So the registers can update
}

/*
// Name: GoToSleep
// Desc: The function that gives the command to go to sleep.
*/
inline static void GoToSleep(void){
	_go_to_sleep();
}

/*
// Name: InitPowerModule
// Desc: Nothing fancy; just sets the power mode to idle.
*/
inline static void InitPowerModule(void){
	SetPowerMode(POWERMODE_IDLE);
}


#endif /* POWER_H_ */