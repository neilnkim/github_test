/*
 * cradle_control.c
 *
 * Created: 2018-06-11 오전 11:18:29
 *  Author: kevin.ko
 */
//#include <string.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "atmel_start.h"
#include "app_config.h"
#include "adc_basic.h"
#include "led_control.h"
#include "cradle_control.h"
#include "driver_init.h"
#include "state.h"


void sleep_wakeup(void)
{
	uart_log("wake up");

	I2C_led_power_on(ID_CRADLE_LED);
	_delay_ms(5);
	I2C_led_power_on(ID_EARBUD_LED);
	_delay_ms(5);
	I2C_set_intensity(ID_CRADLE_LED, CRADLE_LED_CERRENT, CRADLE_LED_CERRENT, CRADLE_LED_CERRENT);
	_delay_ms(5);
	I2C_set_intensity(ID_EARBUD_LED, EARBUD_LED_CERRENT, EARBUD_LED_CERRENT, EARBUD_LED_CERRENT);
	_delay_ms(5);
	I2C_set_color(ID_CRADLE_LED, e_LED_COLOR_OFF);
	_delay_ms(5);
	I2C_set_color(ID_EARBUD_LED, e_LED_COLOR_OFF);
	_delay_ms(5);
}

void sleep_start(void)
{
	uart_log("get into sleep");

	I2C_set_color(ID_CRADLE_LED, e_LED_COLOR_OFF);
	_delay_ms(5);
	I2C_set_color(ID_EARBUD_LED, e_LED_COLOR_OFF);
	_delay_ms(5);
	I2C_led_power_off(ID_EARBUD_LED);
	_delay_ms(5);
	I2C_led_power_off(ID_CRADLE_LED);
	_delay_ms(5);

	wdt_disable();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	cli();
	sei();
	sleep_cpu();		// Sleep Now... z z z z z

	sleep_disable();	// Wake up by external interrupts

	// neil 181112 add watch dog reset : SWT-592
	wdt_enable(WDTO_2S);
	wdt_reset();
}


uint32_t get_tick_1ms(void)//kevin.ko 180730 SWT-499
{
	uint32_t ret;

	ret = g_st.sys_tick_1ms;

	return ret;
}

uint32_t since_1ms(uint32_t tstamp)//kevin.ko 180731 SWT-499
{
	 uint32_t ret, curr;

	 curr = get_tick_1ms();
	 ret = (curr>=tstamp)? (curr-tstamp): ((UINT32_T_MAX-tstamp) + curr);

	 return ret;
}


void set_state_timeout(uint32_t timeout_ms)
{
	g_st.state_timeout = timeout_ms;
	g_st.state_tstamp = get_tick_1ms();
}
/*
uint8_t min_u8(uint8_t a, uint8_t b)
{
	if(a<b) return a;
	else 	return b;
}
uint8_t max_u8(uint8_t a, uint8_t b)
{
	if(a<b) return b;
	else 	return a;
}
*/
void update_battery_led_color(void)
{
	// if earbud mounted
	if(g_st.input_st.mount_earbud[e_LEFT] || g_st.input_st.mount_earbud[e_RIGHT])
	{
		if(   (g_st.input_st.batt_level_earbud[e_LEFT] == e_EARBUD_BATT_LOW) ||
			(g_st.input_st.batt_level_earbud[e_RIGHT] == e_EARBUD_BATT_LOW) )
		{
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[0] = e_LED_COLOR_ORANGE;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[1] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_EARBUD].solid_color 	 = e_LED_COLOR_ORANGE;
		}
		else
		{
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[0] = e_LED_COLOR_GREEN;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[1] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_EARBUD].solid_color 	 = e_LED_COLOR_GREEN;
		}
	}
	// there's no earbud - off
	else
	{
		g_st.led_st[e_WHICH_LED_EARBUD].blink_color[0] = e_LED_COLOR_OFF;
		g_st.led_st[e_WHICH_LED_EARBUD].blink_color[1] = e_LED_COLOR_OFF;
		g_st.led_st[e_WHICH_LED_EARBUD].solid_color	 = e_LED_COLOR_OFF;
	}

	if(g_st.input_st.batt_level_cradle == e_CRADLE_BATT_HIGH)
	{
		g_st.led_st[e_WHICH_LED_CRADLE].blink_color[0] = e_LED_COLOR_GREEN;
		g_st.led_st[e_WHICH_LED_CRADLE].blink_color[1] = e_LED_COLOR_OFF;
		g_st.led_st[e_WHICH_LED_CRADLE].solid_color	 = e_LED_COLOR_GREEN;
	}
	else if(g_st.input_st.batt_level_cradle == e_CRADLE_BATT_MEDIUM)
	{
		g_st.led_st[e_WHICH_LED_CRADLE].blink_color[0] = e_LED_COLOR_ORANGE;
		g_st.led_st[e_WHICH_LED_CRADLE].blink_color[1] = e_LED_COLOR_OFF;
		g_st.led_st[e_WHICH_LED_CRADLE].solid_color	 = e_LED_COLOR_ORANGE;
	}
	else
	{
		g_st.led_st[e_WHICH_LED_CRADLE].blink_color[0] = e_LED_COLOR_RED;
		g_st.led_st[e_WHICH_LED_CRADLE].blink_color[1] = e_LED_COLOR_OFF;
		g_st.led_st[e_WHICH_LED_CRADLE].solid_color	 = e_LED_COLOR_RED;
	}
}

void set_led_action(uint8_t led_idx, uint8_t led_action)
{
	// enable earbud led
	if(g_st.input_st.mount_earbud[e_LEFT] || g_st.input_st.mount_earbud[e_RIGHT])
	{
		g_st.led_st[e_WHICH_LED_EARBUD].action = led_action;
		g_st.led_st[e_WHICH_LED_EARBUD].blink_period = BLINK_SLOW_MS;
	}
	else
	{
		g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_POWER_OFF;
		g_st.led_st[e_WHICH_LED_EARBUD].blink_period = 0;
	}

	// enable cradle led
	g_st.led_st[e_WHICH_LED_CRADLE].action = led_action; // led task looks at action definition of CRADLE
	g_st.led_st[e_WHICH_LED_CRADLE].blink_period = BLINK_SLOW_MS;


	// disable opposite led
	if(led_idx == e_WHICH_LED_CRADLE)
	{
		g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_POWER_OFF;
		g_st.led_st[e_WHICH_LED_EARBUD].blink_period = 0;
	}
	else if(led_idx == e_WHICH_LED_EARBUD)
	{
		g_st.led_st[e_WHICH_LED_CRADLE].action = e_LED_STYLE_POWER_OFF;
		g_st.led_st[e_WHICH_LED_CRADLE].blink_period = 0;
	}
}

void set_led_action_by_state(uint8_t mode)
{
	switch(mode)
	{
		case STATE_PAIR_WAIT_ACK1:
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[0] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[1] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_CRADLE].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_period = BLINK_SLOW_MS;

			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[0] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[1] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_period = BLINK_SLOW_MS;
			break;

		case STATE_PAIR_WAIT_ACK2:
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[0] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[1] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_CRADLE].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_period = BLINK_SLOW_MS;

			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[0] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[1] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_period = BLINK_SLOW_MS;
			break;

		case STATE_PDL_RESET_WAIT_ACK1:
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[0] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[1] = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_CRADLE].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_period = BLINK_SLOW_MS;

			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[0] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[1] = e_LED_COLOR_RED;
			g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_period = BLINK_SLOW_MS;
			break;

		case STATE_PDL_RESET_WAIT_ACK2:
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[0] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_color[1] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_CRADLE].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_period = BLINK_SLOW_MS;

			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[0] = e_LED_COLOR_WHITE;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_color[1] = e_LED_COLOR_RED;
			g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_BLINK;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_period = BLINK_SLOW_MS;
			break;

		case STATE_IDLE:
			g_st.led_st[e_WHICH_LED_CRADLE].solid_color = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_CRADLE].action = e_LED_STYLE_ON_SOLID;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_period = 0;

			g_st.led_st[e_WHICH_LED_EARBUD].solid_color = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_ON_SOLID;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_period = 0;
			break;

		case STATE_POWER_OFF_WAIT_ACK:
			g_st.led_st[e_WHICH_LED_CRADLE].solid_color = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_CRADLE].action = e_LED_STYLE_ON_SOLID;
			g_st.led_st[e_WHICH_LED_CRADLE].blink_period = 0;

			g_st.led_st[e_WHICH_LED_EARBUD].solid_color = e_LED_COLOR_OFF;
			g_st.led_st[e_WHICH_LED_EARBUD].action = e_LED_STYLE_ON_SOLID;
			g_st.led_st[e_WHICH_LED_EARBUD].blink_period = 0;
			break;

		default:
			return;
	}

}

void print_battery_level(void)
{
	uart_log(">>Battery Level/ADC");
	uart_log("  Cradle:%d/%d", g_st.input_st.batt_level_cradle, g_st.input_st.adc_cradle_avr);

	if(g_st.input_st.mount_earbud[e_LEFT])
	{
		uart_log("  ELeft :%d/%d", g_st.input_st.batt_level_earbud[e_LEFT], g_st.input_st.adc_earbud_avr[e_LEFT]);
	}
	else
	{
		uart_log("  ELeft : -");
	}

	if(g_st.input_st.mount_earbud[e_RIGHT])
	{
		uart_log("  ERight:%d/%d", g_st.input_st.batt_level_earbud[e_RIGHT], g_st.input_st.adc_earbud_avr[e_RIGHT]);
	}
	else
	{
		uart_log("  ERight: -");
	}
}

void auto_earbud_power(void)
{
	uart_log(">>Power Control");

	if(g_st.input_st.mount_earbud[e_LEFT] || g_st.input_st.mount_earbud[e_RIGHT])
	{
		E_POW_ON_set_level(true);
		_delay_ms(10);
	}

	if(g_st.input_st.mount_earbud[e_LEFT])
	{
		EL_POW_ON_set_level(true);
		uart_log("  Left ON");
	}
	if(g_st.input_st.mount_earbud[e_RIGHT])
	{
		ER_POW_ON_set_level(true);
		uart_log("  Right ON");
	}

	if( !g_st.input_st.mount_earbud[e_LEFT] && !g_st.input_st.mount_earbud[e_RIGHT] )
	{
		EL_POW_ON_set_level(false); // left
		ER_POW_ON_set_level(false); // right
		E_POW_ON_set_level(false);	// power block
		uart_log("  Power OFF");
	}
}

void manual_earbud_power(bool on)
{
	E_POW_ON_set_level(on);// power block
	EL_POW_ON_set_level(on); // left
	ER_POW_ON_set_level(on); // right

	if(on)
	{
		uart_log("  Power ON Manually");
	}
	else
	{
		uart_log("  Power OFF Manually");
	}
}

void send_one_wire_command(uint8_t earbud, uint8_t cmd)
{
	// pairing command
	// send 150ms click
	if(cmd==e_CMD_PAIR)
	{
		set_one_wire_idle(earbud); //make sure

		set_one_wire_signal(earbud);
		_delay_ms(150);
		set_one_wire_idle(earbud);
	}

	// pdl reset command
	// send 200ms click
	else if(cmd==e_CMD_PDL_RESET)
	{
		set_one_wire_idle(earbud); //make sure

		// neil 181115 : SWT-596
		// modify double tap to single tap
		set_one_wire_signal(earbud);
		_delay_ms(200);
		set_one_wire_idle(earbud);
	}

	// neil 20181203 : SWT-610
	// power off command
	// send 300ms click
	else if(cmd==e_CMD_POWER_OFF)
	{
		set_one_wire_idle(earbud); //make sure

		set_one_wire_signal(earbud);
		_delay_ms(300);
		set_one_wire_idle(earbud);
	}
}


void set_one_wire_io_mode(uint8_t earbud, uint8_t mode)
{
	if(mode == e_INPUT)
	{
		if(earbud==e_BOTH || earbud==e_LEFT)
		{
			EL_CONTROL_set_dir(PORT_DIR_IN);
			EL_CONTROL_set_pull_mode(PORT_PULL_OFF);
			EL_CONTROL_set_isc(PORT_ISC_BOTHEDGES_gc);
		}
		if(earbud==e_BOTH || earbud==e_RIGHT)
		{
			ER_CONTROL_set_dir(PORT_DIR_IN);
			ER_CONTROL_set_pull_mode(PORT_PULL_OFF);
			ER_CONTROL_set_isc(PORT_ISC_BOTHEDGES_gc);
		}
	}
	else //if(mode == e_OUTPUT) - output and LOW
	{
		if(earbud==e_BOTH || earbud==e_LEFT)
		{
			EL_CONTROL_set_dir(PORT_DIR_OUT);
			EL_CONTROL_set_level(e_LOW);
			EL_CONTROL_set_isc(PORT_ISC_BOTHEDGES_gc);
		}
		if(earbud==e_BOTH || earbud==e_RIGHT)
		{
			ER_CONTROL_set_dir(PORT_DIR_OUT);
			ER_CONTROL_set_level(e_LOW);
			ER_CONTROL_set_isc(PORT_ISC_BOTHEDGES_gc);
		}
	}
}

// gpio input
void set_one_wire_idle(uint8_t earbud)
{
	set_one_wire_io_mode(earbud, e_INPUT);
}

// gpio output LOW
void set_one_wire_signal(uint8_t earbud)
{
	set_one_wire_io_mode(earbud, e_OUTPUT);
}

void print_adc(void)
{
#ifdef DEBUG_ON
	printf("adc C, L/R= ");
	printf("%d(%d), ", g_st.input_st.adc_cradle, g_st.input_st.adc_cradle_avr);
	printf("%d(%d)/", g_st.input_st.adc_earbud[e_LEFT], g_st.input_st.adc_earbud_avr[e_LEFT]);
	printf("%d(%d)\r\n", g_st.input_st.adc_earbud[e_RIGHT], g_st.input_st.adc_earbud_avr[e_RIGHT]);
#endif
}

void set_port_input_changed(void)
{
	poll_port_input();

	// mount
	g_st.prev_input_st.mount_earbud[e_LEFT] = !g_st.input_st.mount_earbud[e_LEFT];
	g_st.prev_input_st.mount_earbud[e_RIGHT] = !g_st.input_st.mount_earbud[e_RIGHT];

	// charging - usb plugged
	g_st.prev_input_st.plug_usb = !g_st.input_st.plug_usb;
}

void print_version(void)
{
#ifdef DEBUG_ON
	uart_log("--- MARS CRADLE ---");
	uart_log("v%s", FW_VER);
	uart_log("%s", DATE_RELEASE);
#endif
}

// turn on both LEDs in white - test code to see reboot
void led_show_rebooting(void)
{
	I2C_set_color(e_WHICH_LED_CRADLE, e_LED_COLOR_WHITE);
	I2C_set_color(e_WHICH_LED_EARBUD, e_LED_COLOR_WHITE);

	wait_earbud_charger_ms(3000);
}

void wait_earbud_charger_ms(uint16_t ms)
{
#if 1
	uint32_t tstamp;
	tstamp = get_tick_1ms();

	while(since_1ms(tstamp) <= ms)
	{
		poll_adc(false);
		_delay_ms(1);
		wdt_reset();
	}
#else
	_delay_ms(ms);
#endif
}


void set_cradle_ack(uint8_t idx)
{
	if(idx == e_LEFT)
	{
		g_st.one_wire_st[e_LEFT].event = EVENT_RECEIVE_ACK;
		sbi(g_st.input_st.flag_earbud_got_ack, e_LEFT);
	}
	else if(idx == e_RIGHT)
	{
		g_st.one_wire_st[e_RIGHT].event = EVENT_RECEIVE_ACK;
		sbi(g_st.input_st.flag_earbud_got_ack, e_RIGHT);
	}
	else if(idx == e_BOTH)
	{
		g_st.one_wire_st[e_LEFT].event = EVENT_RECEIVE_ACK;
		g_st.one_wire_st[e_RIGHT].event = EVENT_RECEIVE_ACK;
		g_st.input_st.flag_earbud_got_ack = 0b11;
	}
}


void clear_cradle_ack(uint8_t idx)
{
	if(idx == e_LEFT)
	{
		g_st.one_wire_st[e_LEFT].event = EVENT_NULL;
		cbi(g_st.input_st.flag_earbud_got_ack, e_LEFT);
	}
	else if(idx == e_RIGHT)
	{
		g_st.one_wire_st[e_RIGHT].event = EVENT_NULL;
		cbi(g_st.input_st.flag_earbud_got_ack, e_RIGHT);
	}
	else if(idx == e_BOTH)
	{
		g_st.one_wire_st[e_LEFT].event = EVENT_NULL;
		g_st.one_wire_st[e_RIGHT].event = EVENT_NULL;
		g_st.input_st.flag_earbud_got_ack = 0b00;
	}
}

