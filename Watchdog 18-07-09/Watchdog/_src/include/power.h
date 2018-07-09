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
#define PM						0x40000400
#endif
#define SLEEPMODE_IDLE			0x2
#define SLEEPMODE_STANDBY		0x4
#define VREGSMOD_AUTO			(0x0 << 6)
#define VREGSMOD_PERFORMANCE	(0x1 << 6)
#define VREGSMOD_LP				(0x2 << 6)
#define BBIASHS_BACK_BIAS		(1 << 10)

/// Enums

typedef enum{
	POWERMODE_IDLE		= SLEEPMODE_IDLE,
	POWERMODE_STANDBY	= SLEEPMODE_STANDBY,
} PowerMode;

/// Functions

inline static void SetPowerMode(PowerMode mode){
	hri_pm_set_SLEEPCFG_SLEEPMODE_bf((void*)PM, (uint8)mode);
	delay_ms(2); // So the registers can update
}

inline static void GoToSleep(void){
	_go_to_sleep();
}

// Functions

inline static void InitPowerModule(void){
	SetPowerMode(POWERMODE_IDLE);
}


#endif /* POWER_H_ */