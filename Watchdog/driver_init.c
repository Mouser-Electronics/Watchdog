/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

/*! The buffer size for USART */
#define USART_BUFFER_SIZE 16

struct usart_async_descriptor USART;
struct timer_descriptor       TIMER0;
struct timer_descriptor       TIMER1;
struct timer_descriptor       TIMER2;
struct timer_descriptor       TIMER3;

static uint8_t USART_buffer[USART_BUFFER_SIZE];

struct i2c_m_sync_desc I2C;

void EXTINT_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, CONF_GCLK_EIC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_mclk_set_APBAMASK_EIC_bit(MCLK);

	// Set pin direction to input
	gpio_set_pin_direction(ACCEL, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(ACCEL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_DOWN);

	gpio_set_pin_function(ACCEL, PINMUX_PC24A_EIC_EXTINT0);

	// Set pin direction to input
	gpio_set_pin_direction(BUTTON, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(BUTTON,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_UP);

	gpio_set_pin_function(BUTTON, PINMUX_PB19A_EIC_EXTINT3);

	ext_irq_init();
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void USART_CLOCK_init()
{

	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_SLOW, CONF_GCLK_SERCOM0_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM0_bit(MCLK);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void USART_PORT_init()
{

	gpio_set_pin_function(TX, PINMUX_PA08C_SERCOM0_PAD0);

	gpio_set_pin_function(RX, PINMUX_PA09C_SERCOM0_PAD1);

	gpio_set_pin_function(RTS, PINMUX_PB22C_SERCOM0_PAD2);

	gpio_set_pin_function(CTS, PINMUX_PB23C_SERCOM0_PAD3);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void USART_init(void)
{
	USART_CLOCK_init();
	usart_async_init(&USART, SERCOM0, USART_buffer, USART_BUFFER_SIZE, (void *)NULL);
	USART_PORT_init();
}

void I2C_PORT_init(void)
{

	gpio_set_pin_pull_mode(SDA,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SDA, PINMUX_PA16C_SERCOM1_PAD0);

	gpio_set_pin_pull_mode(SCL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SCL, PINMUX_PA17C_SERCOM1_PAD1);
}

void I2C_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_SLOW, CONF_GCLK_SERCOM1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBCMASK_SERCOM1_bit(MCLK);
}

void I2C_init(void)
{
	I2C_CLOCK_init();
	i2c_m_sync_init(&I2C, SERCOM1);
	I2C_PORT_init();
}

void delay_driver_init(void)
{
	delay_init(SysTick);
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER0_init(void)
{

	hri_mclk_set_APBCMASK_TC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC0_GCLK_ID, CONF_GCLK_TC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	timer_init(&TIMER0, TC0, _tc_get_timer());
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER1_init(void)
{

	hri_mclk_set_APBCMASK_TC2_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC2_GCLK_ID, CONF_GCLK_TC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	timer_init(&TIMER1, TC2, _tc_get_timer());
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER2_init(void)
{

	hri_mclk_set_APBCMASK_TC4_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC4_GCLK_ID, CONF_GCLK_TC4_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	timer_init(&TIMER2, TC4, _tc_get_timer());
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER3_init(void)
{

	hri_mclk_set_APBDMASK_TC6_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC6_GCLK_ID, CONF_GCLK_TC6_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	timer_init(&TIMER3, TC6, _tc_get_timer());
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA10

	// Set pin direction to output
	gpio_set_pin_direction(LED_SHORT, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_SHORT,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LED_SHORT, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA11

	// Set pin direction to output
	gpio_set_pin_direction(LED_MEDIUM, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_MEDIUM,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LED_MEDIUM, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA18

	// Set pin direction to output
	gpio_set_pin_direction(LED_LONG, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_LONG,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LED_LONG, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB00

	// Set pin direction to output
	gpio_set_pin_direction(LED_RED, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_RED,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LED_RED, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB01

	// Set pin direction to output
	gpio_set_pin_direction(LED_GREEN, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_GREEN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LED_GREEN, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB02

	// Set pin direction to output
	gpio_set_pin_direction(LED_BLUE, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_BLUE,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(LED_BLUE, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PC05

	// Set pin direction to output
	gpio_set_pin_direction(LED_BOARD, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(LED_BOARD,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	gpio_set_pin_function(LED_BOARD, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PC27

	// Set pin direction to output
	gpio_set_pin_direction(MOTOR, GPIO_DIRECTION_OUT);

	gpio_set_pin_level(MOTOR,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	gpio_set_pin_function(MOTOR, GPIO_PIN_FUNCTION_OFF);

	EXTINT_init();

	USART_init();

	I2C_init();

	delay_driver_init();

	TIMER0_init();
	TIMER1_init();
	TIMER2_init();
	TIMER3_init();
}
