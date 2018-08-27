/*
 * boardfuncs.h
 *
 * Created: 6/27/2018 11:47:24 AM
 *  Author: braddock.bresnahan
 */ 

// Just some useful definitions to make coding easier

#ifndef BOARDFUNCS_H_
#define BOARDFUNCS_H_

#include <hal_gpio.h>
#include "atmel_start_pins.h"
#include "led.h"

/// Macros

#define ButtonPressed	(!gpio_get_pin_level(BUTTON))

#define DebugOn		(SetLED(LEDD, ON))
#define DebugOff	(SetLED(LEDD, OFF))

#define HIGH	1
#define LOW		0

#define BIT0	(1 << 0)
#define BIT1	(1 << 1)
#define BIT2	(1 << 2)
#define BIT3	(1 << 3)
#define BIT4	(1 << 4)
#define BIT5	(1 << 5)
#define BIT6	(1 << 6)
#define BIT7	(1 << 7)
#define NOBITS	0x00
#define ALLBITS	0xff


#endif /* BOARDFUNCS_H_ */