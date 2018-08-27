/*
 * pin.c
 *
 * Created: 7/13/2018 9:30:53 AM
 *  Author: braddock.bresnahan
 */ 

#include "led.h"
#include "driver_init.h"
#include "timers.h"

/// Functions

bool GetLED(uint8 ledMask){
	bool state = false;
	
	if (ledMask & LEDD)
		state = state || !gpio_get_pin_level(LED_BOARD);
	if (ledMask & LEDS)
		state = state || gpio_get_pin_level(LED_SHORT);
	if (ledMask & LEDM)
		state = state || gpio_get_pin_level(LED_MEDIUM);
	if (ledMask & LEDL)
		state = state || gpio_get_pin_level(LED_LONG);
	if (ledMask & LEDR)
		state = state || gpio_get_pin_level(LED_RED);
	if (ledMask & LEDG)
		state = state || gpio_get_pin_level(LED_GREEN);
	if (ledMask & LEDB)
		state = state || gpio_get_pin_level(LED_BLUE);
		
	return state;
}

void SetLED(uint8 ledMask, bool state){
	if (ledMask & LEDD)
		gpio_set_pin_level(LED_BOARD, !state);
	if (ledMask & LEDS)
		gpio_set_pin_level(LED_SHORT, state);
	if (ledMask & LEDM)
		gpio_set_pin_level(LED_MEDIUM, state);
	if (ledMask & LEDL)
		gpio_set_pin_level(LED_LONG, state);
	if (ledMask & LEDR)
		gpio_set_pin_level(LED_RED, state);
	if (ledMask & LEDG)
		gpio_set_pin_level(LED_GREEN, state);
	if (ledMask & LEDB)
		gpio_set_pin_level(LED_BLUE, state);
}

uint8 GetActiveLEDs(){
	uint8 active = 0;
	
	if (!gpio_get_pin_level(LED_BOARD))
		active |= LEDD;
	if (gpio_get_pin_level(LED_SHORT))
		active |= LEDS;
	if (gpio_get_pin_level(LED_MEDIUM))
		active |= LEDM;
	if (gpio_get_pin_level(LED_LONG))
		active |= LEDL;
	if (gpio_get_pin_level(LED_RED))
		active |= LEDR;
	if (gpio_get_pin_level(LED_GREEN))
		active |= LEDG;
	if (gpio_get_pin_level(LED_BLUE))
		active |= LEDB;
		
	return active;
}

/// LED Section

void LEDEnqueue(uint8 ledMask, const char* const instructions){
	static struct timer_task task = { .cb = TIMER3_CALLBACK, .mode = TIMER_TASK_ONE_SHOT, .interval = 1, };
	static struct QueueInst inst;
	static const char* cursor;
	
	inst.misc = ledMask;
	cursor = instructions;
	while (*cursor != '\0'){
		switch (*cursor){
			case '-':
			case '+':
			case ',':
			case '^':
			case '*': inst.type = INST_LED_ON; break;
			case '_':
			case '=':
			case '.':
			case '%':
			case '/': inst.type = INST_LED_OFF; break;
			default: inst.type = INST_NOP; break;
		}
		QUE_Enqueue(&task4, &inst);
		cursor++;
	}
	
	if (!_timer_is_started(&TIMER3.device) && !QUE_IsEmpty(&task4)){
		timer_add_task(&TIMER3, &task);
		timer_start(&TIMER3);
	}
}

void LEDClearQueue(){
	QUE_Clear(&task4);
}

bool LEDIsIdle(){
	return QUE_IsEmpty(&task4) && !_timer_is_started(&TIMER3.device);
}

void LEDDelay(uint milliseconds){
	uint delay = milliseconds / TIMER3_DURATION;
	char* const inst = malloc(delay + 1);
	
	for (uint i = 0; i < delay; i++)
		inst[i] = '$'; // NOP
	inst[delay] = '\0';
	LEDEnqueue(0, inst);
	
	free(inst);
}

void LEDBlink(uint8 ledMask, uint length){
	uint reqLength = GetDurationOfTimer(&TIMER0)->length + 1;
	uint delayLength = length - TIMER3_DURATION;
	
	while (task4.size < reqLength){
		LEDEnqueue(ledMask, "+");
		LEDDelay(delayLength);
		LEDEnqueue(ledMask, "=");
		LEDDelay(delayLength);
	}
}

void LEDBlinkAdv(uint8 ledMask, uint length, float duration){
	uint reqLength = (uint)((float)GetDurationOfTimer(&TIMER0)->length * duration) + 1;
	uint delayLength = length - TIMER3_DURATION;
	
	while (task4.size < reqLength){
		LEDEnqueue(ledMask, "+");
		LEDDelay(delayLength);
		LEDEnqueue(ledMask, "=");
		LEDDelay(delayLength);
	}
}