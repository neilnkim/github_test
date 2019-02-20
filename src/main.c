#include <atmel_start.h>
#include <util/delay.h>
#include <atomic.h>
#include <stdio.h>
#include <avr/wdt.h>
#include "app_config.h"
#include "app_init.h"
#include "led_control.h"
#include "cradle_control.h"
#include "state.h"


st_status_t	g_st;

int main(void)
{
	atmel_start_init();
	app_init();

#ifdef DEBUG_ON
	print_version();
	led_show_rebooting();
#endif

	while (1)
	{
		poll_status();

		task_led();
		task_states();
	}
}

