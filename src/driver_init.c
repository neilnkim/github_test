/**
 * \file
 *
 * \brief Driver initialization.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/*
 * Code generated by START.
 *
 * This file will be overwritten when reconfiguring your START project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "app_config.h"
#include "driver_init.h"
#include "clock_config.h"
#include <system.h>


/*System 1mS tick*/
// neil 20181108
// modify to accurate calculation - SWT-584
#define CLOCK_KHZ		1000
#define SYS_TICK_TC_TOP  ((F_CPU / CLOCK_KHZ) +1)


/* Configure pins and initialize registers */
void ADC_0_initialization(void)
{

	// Disable digital input buffer
	PA1_set_isc(PORT_ISC_INPUT_DISABLE_gc);
	// Disable pull-up resistor
	PA1_set_pull_mode(PORT_PULL_OFF);

	// Disable digital input buffer
	PA2_set_isc(PORT_ISC_INPUT_DISABLE_gc);
	// Disable pull-up resistor
	PA2_set_pull_mode(PORT_PULL_OFF);

	// Disable digital input buffer
	PA3_set_isc(PORT_ISC_INPUT_DISABLE_gc);
	// Disable pull-up resistor
	PA3_set_pull_mode(PORT_PULL_OFF);

	ADC_0_init();
}

/* configure the pins and initialize the registers */
void USART_0_initialization(void)
{

	// Set pin direction to input
	PB3_set_dir(PORT_DIR_IN);

	PB3_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_OFF);

	// Set pin direction to output
	PB2_set_dir(PORT_DIR_OUT);

	PB2_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	USART_0_init();
}


void TIMER_0_initialization(void)//kevin.ko 180730 SWT-499
{

	/* TCA used as 1mS tick for system timing */
	TCA0.SINGLE.PER = SYS_TICK_TC_TOP;
	/*Normal mode*/
	TCA0.SINGLE.CTRLB = 0x00;
	/*Enable overflow interrupt*/
	TCA0.SINGLE.INTCTRL = 0x01;
	/*Enable TCA*/
	TCA0.SINGLE.CTRLA = 0x01;
}


/* configure pins and initialize registers */
void I2C_0_initialization(void)
{

	PB0_set_dir(
	// <y> Pin direction
	// <id> pad_dir
	// <PORT_DIR_OFF"> Off
	// <PORT_DIR_IN"> In
	// <PORT_DIR_OUT"> Out
	PORT_DIR_OUT);

	PB0_set_level(
	// <y> Initial level
	// <id> pad_initial_level
	// <false"> Low
	// <true"> High
	false);

	PB0_set_pull_mode(
	// <y> Pull configuration
	// <id> pad_pull_config
	// <PORT_PULL_OFF"> Off
	// <PORT_PULL_UP"> Pull-up
	PORT_PULL_OFF);

	PB0_set_inverted(
	// <y> Invert I/O on pin
	// <id> pad_invert
	// <false"> Not inverted
	// <true"> Inverted
	false);

	PB0_set_isc(
	// <y> Pin Input/Sense Configuration
	// <id> pad_isc
	// <PORT_ISC_INTDISABLE_gc"> Interrupt disabled but input buffer enabled
	// <PORT_ISC_BOTHEDGES_gc"> Sense Both Edges
	// <PORT_ISC_RISING_gc"> Sense Rising Edge
	// <PORT_ISC_FALLING_gc"> Sense Falling Edge
	// <PORT_ISC_INPUT_DISABLE_gc"> Digital Input Buffer disabled
	// <PORT_ISC_LEVEL_gc"> Sense low Level
	PORT_ISC_INTDISABLE_gc);

	PB1_set_dir(
	// <y> Pin direction
	// <id> pad_dir
	// <PORT_DIR_OFF"> Off
	// <PORT_DIR_IN"> In
	// <PORT_DIR_OUT"> Out
	PORT_DIR_OUT);

	PB1_set_level(
	// <y> Initial level
	// <id> pad_initial_level
	// <false"> Low
	// <true"> High
	false);

	PB1_set_pull_mode(
	// <y> Pull configuration
	// <id> pad_pull_config
	// <PORT_PULL_OFF"> Off
	// <PORT_PULL_UP"> Pull-up
	PORT_PULL_OFF);

	PB1_set_inverted(
	// <y> Invert I/O on pin
	// <id> pad_invert
	// <false"> Not inverted
	// <true"> Inverted
	false);

	PB1_set_isc(
	// <y> Pin Input/Sense Configuration
	// <id> pad_isc
	// <PORT_ISC_INTDISABLE_gc"> Interrupt disabled but input buffer enabled
	// <PORT_ISC_BOTHEDGES_gc"> Sense Both Edges
	// <PORT_ISC_RISING_gc"> Sense Rising Edge
	// <PORT_ISC_FALLING_gc"> Sense Falling Edge
	// <PORT_ISC_INPUT_DISABLE_gc"> Digital Input Buffer disabled
	// <PORT_ISC_LEVEL_gc"> Sense low Level
	PORT_ISC_INTDISABLE_gc);

	I2C_0_init();
}

/**
 * \brief System initialization
 */
void system_init()
{
	mcu_init();

#ifdef ONE_WIRE_LOW_ACTIVE
	/* PORT setting on PA4 */
	EL_CONTROL_set_dir(PORT_DIR_IN);
	EL_CONTROL_set_pull_mode(PORT_PULL_OFF); // externally pulled up

	/* PORT setting on PA5 */
	ER_CONTROL_set_dir(PORT_DIR_OUT);
	ER_CONTROL_set_pull_mode(PORT_PULL_OFF); // externally pulled up

#else
	/* PORT setting on PA4 */
	EL_CONTROL_set_dir(PORT_DIR_IN);
	EL_CONTROL_set_level(PORT_LOW);
	EL_CONTROL_set_pull_mode(PORT_PULL_OFF);
	EL_CONTROL_set_isc(PORT_ISC_BOTHEDGES_gc);

	/* PORT setting on PA5 */
	ER_CONTROL_set_dir(PORT_DIR_OUT);
	ER_CONTROL_set_level(PORT_LOW);
	ER_CONTROL_set_pull_mode(PORT_PULL_OFF);
	ER_CONTROL_set_isc(PORT_ISC_BOTHEDGES_gc);

#endif

	/* PORT setting on PA6 */
	ER_INSERT_set_dir(
	    // <y> Pin direction
	    // <id> pad_dir
	    // <PORT_DIR_OFF"> Off
	    // <PORT_DIR_IN"> In
	    // <PORT_DIR_OUT"> Out
	    PORT_DIR_IN);

	ER_INSERT_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    true);

	ER_INSERT_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_UP);

	ER_INSERT_set_inverted(
	    // <y> Invert I/O on pin
	    // <id> pad_invert
	    // <false"> Not inverted
	    // <true"> Inverted
	    false);

	ER_INSERT_set_isc(
	    // <y> Pin Input/Sense Configuration
	    // <id> pad_isc
	    // <PORT_ISC_INTDISABLE_gc"> Interrupt disabled but input buffer enabled
	    // <PORT_ISC_BOTHEDGES_gc"> Sense Both Edges
	    // <PORT_ISC_RISING_gc"> Sense Rising Edge
	    // <PORT_ISC_FALLING_gc"> Sense Falling Edge
	    // <PORT_ISC_INPUT_DISABLE_gc"> Digital Input Buffer disabled
	    // <PORT_ISC_LEVEL_gc"> Sense low Level
	  PORT_ISC_BOTHEDGES_gc);//edit kevin.ko 18-06-08

	/* PORT setting on PA7 */

	EL_INSERT_set_dir(
	    // <y> Pin direction
	    // <id> pad_dir
	    // <PORT_DIR_OFF"> Off
	    // <PORT_DIR_IN"> In
	    // <PORT_DIR_OUT"> Out
	    PORT_DIR_IN);

	EL_INSERT_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    true);

	EL_INSERT_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_UP);

	EL_INSERT_set_inverted(
	    // <y> Invert I/O on pin
	    // <id> pad_invert
	    // <false"> Not inverted
	    // <true"> Inverted
	    false);

	EL_INSERT_set_isc(
	    // <y> Pin Input/Sense Configuration
	    // <id> pad_isc
	    // <PORT_ISC_INTDISABLE_gc"> Interrupt disabled but input buffer enabled
	    // <PORT_ISC_BOTHEDGES_gc"> Sense Both Edges
	    // <PORT_ISC_RISING_gc"> Sense Rising Edge
	    // <PORT_ISC_FALLING_gc"> Sense Falling Edge
	    // <PORT_ISC_INPUT_DISABLE_gc"> Digital Input Buffer disabled
	    // <PORT_ISC_LEVEL_gc"> Sense low Level
	  PORT_ISC_BOTHEDGES_gc);//edit kevin.ko 18-06-08


	/* PORT setting on PB4 */

	USB_INSERT_set_dir(
	    // <y> Pin direction
	    // <id> pad_dir
	    // <PORT_DIR_OFF"> Off
	    // <PORT_DIR_IN"> In
	    // <PORT_DIR_OUT"> Out
	    PORT_DIR_IN);

	USB_INSERT_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	USB_INSERT_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_OFF);

	USB_INSERT_set_inverted(
	    // <y> Invert I/O on pin
	    // <id> pad_invert
	    // <false"> Not inverted
	    // <true"> Inverted
	    false);

	USB_INSERT_set_isc(
	    // <y> Pin Input/Sense Configuration
	    // <id> pad_isc
	    // <PORT_ISC_INTDISABLE_gc"> Interrupt disabled but input buffer enabled
	    // <PORT_ISC_BOTHEDGES_gc"> Sense Both Edges
	    // <PORT_ISC_RISING_gc"> Sense Rising Edge
	    // <PORT_ISC_FALLING_gc"> Sense Falling Edge
	    // <PORT_ISC_INPUT_DISABLE_gc"> Digital Input Buffer disabled
	    // <PORT_ISC_LEVEL_gc"> Sense low Level
		PORT_ISC_BOTHEDGES_gc); //edit kevin.ko 18-06-08

	/* PORT setting on PB5 */

	FUNC_BTN_set_dir(
	    // <y> Pin direction
	    // <id> pad_dir
	    // <PORT_DIR_OFF"> Off
	    // <PORT_DIR_IN"> In
	    // <PORT_DIR_OUT"> Out
	    PORT_DIR_IN);

	FUNC_BTN_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	FUNC_BTN_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_UP);

	FUNC_BTN_set_inverted(
	    // <y> Invert I/O on pin
	    // <id> pad_invert
	    // <false"> Not inverted
	    // <true"> Inverted
	    false);

	FUNC_BTN_set_isc(
	    // <y> Pin Input/Sense Configuration
	    // <id> pad_isc
	    // <PORT_ISC_INTDISABLE_gc"> Interrupt disabled but input buffer enabled
	    // <PORT_ISC_BOTHEDGES_gc"> Sense Both Edges
	    // <PORT_ISC_RISING_gc"> Sense Rising Edge
	    // <PORT_ISC_FALLING_gc"> Sense Falling Edge
	    // <PORT_ISC_INPUT_DISABLE_gc"> Digital Input Buffer disabled
	    // <PORT_ISC_LEVEL_gc"> Sense low Level
		PORT_ISC_BOTHEDGES_gc);//edit kevin.ko 18-06-08

	/* PORT setting on PC0 */

	// Set pin direction to output
	ER_POW_ON_set_dir(PORT_DIR_OUT);

	ER_POW_ON_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PC1 */

	// Set pin direction to output
	EL_POW_ON_set_dir(PORT_DIR_OUT);

	EL_POW_ON_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PC2 */

	// Set pin direction to output
	E_POW_ON_set_dir(PORT_DIR_OUT);

	E_POW_ON_set_level(
	    // <y> Initial level
	    // <id> pad_initial_level
	    // <false"> Low
	    // <true"> High
	    false);

	/* PORT setting on PC3 */

	// Set pin direction to input
	BAT_STAT_set_dir(PORT_DIR_IN);

	BAT_STAT_set_pull_mode(
	    // <y> Pull configuration
	    // <id> pad_pull_config
	    // <PORT_PULL_OFF"> Off
	    // <PORT_PULL_UP"> Pull-up
	    PORT_PULL_OFF);

	ADC_0_initialization();

#ifdef DEBUG_ON
	USART_0_initialization();
#endif

	TIMER_0_initialization();

	CPUINT_init();

	SLPCTRL_init();

	I2C_0_initialization();

}
