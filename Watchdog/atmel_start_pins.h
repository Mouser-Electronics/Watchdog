/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMC21 has 9 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7
#define GPIO_PIN_FUNCTION_I 8

#define TX GPIO(GPIO_PORTA, 8)
#define RX GPIO(GPIO_PORTA, 9)
#define LED_SHORT GPIO(GPIO_PORTA, 10)
#define LED_MEDIUM GPIO(GPIO_PORTA, 11)
#define SDA GPIO(GPIO_PORTA, 16)
#define SCL GPIO(GPIO_PORTA, 17)
#define LED_LONG GPIO(GPIO_PORTA, 18)
#define LED_RED GPIO(GPIO_PORTB, 0)
#define LED_GREEN GPIO(GPIO_PORTB, 1)
#define LED_BLUE GPIO(GPIO_PORTB, 2)
#define BUTTON GPIO(GPIO_PORTB, 19)
#define RTS GPIO(GPIO_PORTB, 22)
#define CTS GPIO(GPIO_PORTB, 23)
#define LED_BOARD GPIO(GPIO_PORTC, 5)
#define ACCEL GPIO(GPIO_PORTC, 24)
#define MOTOR GPIO(GPIO_PORTC, 27)

#endif // ATMEL_START_PINS_H_INCLUDED
