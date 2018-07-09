/*
 * boardfuncs.h
 *
 * Created: 6/27/2018 11:47:24 AM
 *  Author: braddock.bresnahan
 */ 


#ifndef BOARDFUNCS_H_
#define BOARDFUNCS_H_

#include <hal_gpio.h>
#include "atmel_start_pins.h"

/// Macros

#define ON				true
#define OFF				false

#define ButtonPressed	(!gpio_get_pin_level(BUTTON))
#define LEDLit			(!gpio_get_pin_level(LED))
#define SetLED(pin, x)	(gpio_set_pin_level(pin, x))
#define ToggleLED(pin)  (gpio_toggle_pin_level(pin))

#define SetMotor(x)		(gpio_set_pin_level(MOTOR, x))
#define MotorRunning	(gpio_get_pin_level(MOTOR))

#define DebugOn		(SetLED(LED, OFF))
#define DebugOff	(SetLED(LED, ON))

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