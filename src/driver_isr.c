/**
 * \file
 *
 * \brief Driver ISR.
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

#include <driver_init.h>
#include <compiler.h>
#include <avr/interrupt.h>
#include "cradle_control.h"
#include "led_control.h"
#include "i2c_master.h"
#include "app_config.h"


volatile uint8_t int_flag;

ISR(PORTA_PORT_vect)
{
	/* Insert your PORTA interrupt handling code here */
	int_flag = PORTA.INTFLAGS;

	if((int_flag >> 4) & 0x01)	// Earbud L control
	{
	}
	if((int_flag >> 5) & 0x01)	// Earbud R control
	{
	}
	if((int_flag >> 6) & 0x01) // Earbud R insert
	{
	}
	if((int_flag >> 7) & 0x01) // Earbud L insert
	{
	}

	/* Clear interrupt flags */
	PORTA.INTFLAGS = int_flag;
}

ISR(PORTB_PORT_vect)
{
	/* Insert your PORTB interrupt handling code here */
	int_flag = PORTB.INTFLAGS;

	if((int_flag >> 4) & 0x01) //USB insert interrupt
	{
	}
	if((int_flag >> 5) & 0x01) //Button push & pull interrupt
	{
	}

	/* Clear interrupt flags */
	PORTB.INTFLAGS = int_flag;
}


ISR(TCA0_OVF_vect)
{
	/*Clear OVF flag*/
	TCA0.SINGLE.INTFLAGS = 0x01;
//	TCA0.INTFLAGS = 0x01;

	g_st.sys_tick_1ms++;
}

