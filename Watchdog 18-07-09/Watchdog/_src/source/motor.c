/*
 * motor.c
 *
 * Created: 6/28/2018 8:06:33 AM
 *  Author: braddock.bresnahan
 */ 

#include "motor.h"
#include "driver_init.h"
#include "timers.h"

/// Functions

void MotorEnqueue(const char* const instruction){
	static struct timer_task task = { .cb = TIMER_CALLBACK, .mode = TIMER_TASK_ONE_SHOT, .interval = 1, };
	const char* temp = instruction;
	static struct QueueInst inst;
	
	temp = instruction;
	while (*temp != '\0'){
		switch (*temp){
			case '_':
			case '=':
			case '.':
			case '%':
			case '/': inst.type = INST_MOTOR_REST; break;
			case '+':
			case '*': inst.type = INST_MOTOR_PRIME; break;
			case '-': inst.type = INST_MOTOR_ACTIVE; break;
			case ',': inst.type = INST_MOTOR_WEAK; break;
			case '^': inst.type = INST_MOTOR_SHAKE; break;
			default: inst.type = INST_NOP; break;
		}
		QUE_Enqueue(&task3, &inst);
		temp++;
	}
	
	if (!_timer_is_started(&TIMER3.device) && !QUE_IsEmpty(&task3)){
		timer_add_task(&TIMER3, &task);
		timer_start(&TIMER3);
	}
}

void MotorClearQueue(){
	QUE_Clear(&task3);
}

bool MotorIsIdle(){
	return QUE_IsEmpty(&task3) && !_timer_is_started(&TIMER3.device);
}

void MotorDelay(uint milliseconds){
	uint delay = milliseconds / TIMER_DURATION;
	char* const inst = malloc(delay + 1);
	
	for (uint i = 0; i < delay; i++)
		inst[i] = '$'; // NOP
	inst[delay] = '\0';
	MotorEnqueue(inst);
		
	free(inst);
}