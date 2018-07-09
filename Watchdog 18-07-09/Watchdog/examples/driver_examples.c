/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

static void button_on_PC24_pressed(void)
{
}

static void button_on_PB19_pressed(void)
{
}

/**
 * Example of using EXTINT
 */
void EXTINT_example(void)
{

	ext_irq_register(PIN_PC24, button_on_PC24_pressed);
	ext_irq_register(PIN_PB19, button_on_PB19_pressed);
}

void I2C_example(void)
{
	struct io_descriptor *I2C_io;

	i2c_m_sync_get_io_descriptor(&I2C, &I2C_io);
	i2c_m_sync_enable(&I2C);
	i2c_m_sync_set_slaveaddr(&I2C, 0x12, I2C_M_SEVEN);
	io_write(I2C_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using USART to write "Hello World" using the IO abstraction.
 */
void USART_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART, &io);
	usart_sync_enable(&USART);

	io_write(io, (uint8_t *)"Hello World!", 12);
}

void delay_example(void)
{
	delay_ms(5000);
}

static struct timer_task TIMER0_task1, TIMER0_task2;

/**
 * Example of using TIMER0.
 */
static void TIMER0_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER0_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER0_example(void)
{
	TIMER0_task1.interval = 100;
	TIMER0_task1.cb       = TIMER0_task1_cb;
	TIMER0_task1.mode     = TIMER_TASK_REPEAT;
	TIMER0_task2.interval = 200;
	TIMER0_task2.cb       = TIMER0_task2_cb;
	TIMER0_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER0, &TIMER0_task1);
	timer_add_task(&TIMER0, &TIMER0_task2);
	timer_start(&TIMER0);
}

static struct timer_task TIMER1_task1, TIMER1_task2;

/**
 * Example of using TIMER1.
 */
static void TIMER1_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER1_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER1_example(void)
{
	TIMER1_task1.interval = 100;
	TIMER1_task1.cb       = TIMER1_task1_cb;
	TIMER1_task1.mode     = TIMER_TASK_REPEAT;
	TIMER1_task2.interval = 200;
	TIMER1_task2.cb       = TIMER1_task2_cb;
	TIMER1_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER1, &TIMER1_task1);
	timer_add_task(&TIMER1, &TIMER1_task2);
	timer_start(&TIMER1);
}

static struct timer_task TIMER2_task1, TIMER2_task2;

/**
 * Example of using TIMER2.
 */
static void TIMER2_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER2_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER2_example(void)
{
	TIMER2_task1.interval = 100;
	TIMER2_task1.cb       = TIMER2_task1_cb;
	TIMER2_task1.mode     = TIMER_TASK_REPEAT;
	TIMER2_task2.interval = 200;
	TIMER2_task2.cb       = TIMER2_task2_cb;
	TIMER2_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER2, &TIMER2_task1);
	timer_add_task(&TIMER2, &TIMER2_task2);
	timer_start(&TIMER2);
}

static struct timer_task TIMER3_task1, TIMER3_task2;

/**
 * Example of using TIMER3.
 */
static void TIMER3_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER3_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER3_example(void)
{
	TIMER3_task1.interval = 100;
	TIMER3_task1.cb       = TIMER3_task1_cb;
	TIMER3_task1.mode     = TIMER_TASK_REPEAT;
	TIMER3_task2.interval = 200;
	TIMER3_task2.cb       = TIMER3_task2_cb;
	TIMER3_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER3, &TIMER3_task1);
	timer_add_task(&TIMER3, &TIMER3_task2);
	timer_start(&TIMER3);
}
