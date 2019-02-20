/*
 * app_init.c
 *
 * Created: 2018-06-10 오후 3:57:55
 *  Author: kevin.ko
 */
#include <util/delay.h>
#include <avr/wdt.h>
#include "app_config.h"
#include "app_init.h"
#include "led_control.h"
#include "cradle_control.h"
#include "state.h"

void app_init(void)
{
	set_one_wire_io_mode(e_BOTH, e_INPUT);
	sleep_wakeup();

	g_st.input_st.batt_level_cradle = e_CRADLE_BATT_MEDIUM;
	g_st.input_st.batt_level_earbud[e_LEFT] = e_EARBUD_BATT_LOW;
	g_st.input_st.batt_level_earbud[e_RIGHT] = e_EARBUD_BATT_LOW;

	set_state_timeout(10000);

	// neil 181112 add watch dog reset : SWT-592
	wdt_enable(WDTO_2S);
	wdt_reset();
}

