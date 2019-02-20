/*
 * state.c
 *
 * Created: 2018-10-18 PM 12:21:03
 *  Author: neil
 */


#include <string.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include "app_config.h"
#include "state.h"
#include "cradle_control.h"
#include "led_control.h"


static const state_functions_t state_table[NUM_STATES] =
{
	{	// STATE_IDLE,
		.entry_function = state_entry_idle,
		.action_function = state_action_idle,
		.exit_function = state_exit_idle
	},
	{	// STATE_CRADLE_CHARGING,
		.entry_function = state_entry_cradle_charging,
		.action_function = state_action_cradle_charging,
		.exit_function = state_exit_cradle_charging
	},
	{	// STATE_BATT_DISPLAY,
		.entry_function = state_entry_batt_display,
		.action_function = state_action_batt_display,
		.exit_function = state_exit_batt_display
	},
	{	// STATE_PAIR_WAIT_ACK1,
		.entry_function = state_entry_pair_wait1,
		.action_function = state_action_pair_wait1,
		.exit_function = state_exit_pair_wait1
	},
	{	// STATE_PAIR_WAIT_ACK2,
		.entry_function = state_entry_pair_wait2,
		.action_function = state_action_pair_wait2,
		.exit_function = state_exit_pair_wait2
	},
	{	// STATE_PDL_RESET_WAIT_ACK1,
		.entry_function = state_entry_pdl_reset_wait1,
		.action_function = state_action_pdl_reset_wait1,
		.exit_function = state_exit_pdl_reset_wait1
	},
	{	// STATE_PDL_RESET_WAIT_ACK2,
		.entry_function = state_entry_pdl_reset_wait2,
		.action_function = state_action_pdl_reset_wait2,
		.exit_function = state_exit_pdl_reset_wait2
	},
	{	// STATE_POWER_OFF_ACK,
		.entry_function = state_entry_power_off_wait,
		.action_function = state_action_power_off_wait,
		.exit_function = state_exit_power_off_wait
	},
};


// -------------------------------------------------------------
state_t state_entry_idle(event_t event)
{
	uart_log("state_entry_idle");

	set_led_action_by_state(STATE_IDLE);
	set_state_timeout(5000);

	return NO_STATE_CHANGE;
}

state_t state_action_idle(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			// re-enter this state
			return STATE_CRADLE_CHARGING;

		case EVENT_USB_UNPLUG:
			return STATE_IDLE;

		case EVENT_EARBUD_MOUNT:
			//neil 181115 : SWT-595
			if(g_st.input_st.mount_earbud[e_LEFT] || g_st.input_st.mount_earbud[e_RIGHT])
			{
				g_st.flag_target_blink_led = e_WHICH_LED_EARBUD;
				return STATE_BATT_DISPLAY;
			}
			else
				return NO_STATE_CHANGE;

		case EVENT_BUTTON_CLICK:
			//neil 181115 : SWT-595
			g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
			return STATE_BATT_DISPLAY;

		case EVENT_BUTTON_3S_HOLD:
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;

		case EVENT_RECEIVE_ACK:
			// got fully charged from earbud:  SWT-610
			// cradle will control earbud to power off
			if(g_st.input_st.flag_earbud_mount)
				return STATE_POWER_OFF_WAIT_ACK;
			else
				return NO_STATE_CHANGE;

		case EVENT_STATE_TIMEOUT:
			sleep_start(); 	// go sleep
			// -------------------------------
			sleep_wakeup();	// wake up with external interrupt
			// after wake up
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_idle(event_t event)
{
	uart_log("state_exit_idle");

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}

// -------------------------------------------------------------
// usb charger plugged - led on
state_t state_entry_cradle_charging(event_t event)
{
	uart_log("state_entry_cradle_charging");

	set_port_input_changed();
	poll_adc(true);
	auto_earbud_power(); // dont forget to supply power after pairing
	update_battery_led_color();
	print_battery_level();

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_ON_SOLID);
	set_state_timeout(0); // 0 means no time out limit

	return NO_STATE_CHANGE;
}

state_t state_action_cradle_charging(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			return NO_STATE_CHANGE;

		case EVENT_USB_UNPLUG:
			return STATE_IDLE;

		case EVENT_EARBUD_MOUNT:
			// re-enter this state
			return STATE_CRADLE_CHARGING;

		case EVENT_BUTTON_CLICK:
			// re-enter this state
			return STATE_CRADLE_CHARGING;

		case EVENT_BUTTON_3S_HOLD:
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;

		case EVENT_RECEIVE_ACK:
			// got fully charged from earbud:  SWT-610
			// cradle will control earbud to power off
			if(g_st.input_st.flag_earbud_mount)
				return STATE_POWER_OFF_WAIT_ACK;
			else
				return NO_STATE_CHANGE;

		case EVENT_STATE_TIMEOUT:
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			update_battery_led_color();
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_cradle_charging(event_t event)
{
	uart_log("state_exit_cradle_charging");

	// power off or black color?
	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}


// -------------------------------------------------------------
// battery display - blink led for 3 seconds
state_t state_entry_batt_display(event_t event)
{
	uart_log("state_entry_batt_display");

	poll_port_input();
	poll_adc(true);
	auto_earbud_power(); // dont forget to supply power after pairing
	print_battery_level();

	update_battery_led_color();

	set_led_action(g_st.flag_target_blink_led, e_LED_STYLE_BLINK);
	set_state_timeout(3000);

	return NO_STATE_CHANGE;
}

state_t state_action_batt_display(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			return STATE_CRADLE_CHARGING;

		case EVENT_USB_UNPLUG:
			return NO_STATE_CHANGE;

		case EVENT_EARBUD_MOUNT:
			// re-enter this state
			g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
			return STATE_BATT_DISPLAY;

		case EVENT_BUTTON_CLICK:
			// re-enter this state
			g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
			return STATE_BATT_DISPLAY;

		case EVENT_BUTTON_3S_HOLD:
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;


		case EVENT_RECEIVE_ACK:
			// got fully charged from earbud:  SWT-610
			// cradle will control earbud to power off
			if(g_st.input_st.flag_earbud_mount)
				return STATE_POWER_OFF_WAIT_ACK;
			else
				return NO_STATE_CHANGE;

		case EVENT_STATE_TIMEOUT:
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			update_battery_led_color();
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_batt_display(event_t event)
{
	uart_log("state_exit_batt_display");

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}


// -------------------------------------------------------------
// send pair command to earbud and wait ack
state_t state_entry_pair_wait1(event_t event)
{
	uart_log("state_entry_pair_wait1");

	send_one_wire_command(e_BOTH, e_CMD_PAIR);
	set_state_timeout(300000); // timeout=5min
	set_led_action_by_state(STATE_PAIR_WAIT_ACK1);

	clear_cradle_ack(e_BOTH);

	return NO_STATE_CHANGE;
}

state_t state_action_pair_wait1(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			return STATE_CRADLE_CHARGING;

		case EVENT_USB_UNPLUG:
			return STATE_IDLE;

		case EVENT_EARBUD_MOUNT:
			if(g_st.input_st.plug_usb)
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_EARBUD; //neil 181115 : SWT-595
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_CLICK:
			if(g_st.input_st.plug_usb)
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_3S_HOLD:
			// re-enter thins state
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;

		case EVENT_RECEIVE_ACK:
			// both mount, both ack
			if( (g_st.input_st.flag_earbud_got_ack & g_st.input_st.flag_earbud_mount) == 0x03 )
			{
				uart_log("got pair ack1 from both");
				return STATE_PAIR_WAIT_ACK2;
			}

			if( g_st.input_st.flag_earbud_got_ack & BIT(e_LEFT) )
			{
				uart_log("got pair ack1 from left");
			}
			if( g_st.input_st.flag_earbud_got_ack & BIT(e_RIGHT) )
			{
				uart_log("got pair ack1 from right");
			}
			return NO_STATE_CHANGE;

		case EVENT_STATE_TIMEOUT:
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_pair_wait1(event_t event)
{
	uart_log("state_exit_pair_wait1");

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}


// -------------------------------------------------------------
state_t state_entry_pair_wait2(event_t event)
{
	uart_log("state_entry_pair_wait2");

	set_state_timeout(300000); // timeout=5min
	set_led_action_by_state(STATE_PAIR_WAIT_ACK2);

	// cut off power supply for earbud to wake up
	manual_earbud_power(false); //neil 181115 : SWT-596
	clear_cradle_ack(e_BOTH);

	return NO_STATE_CHANGE;
}

state_t state_action_pair_wait2(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			return STATE_CRADLE_CHARGING;

		case EVENT_USB_UNPLUG:
			return STATE_IDLE;

		case EVENT_EARBUD_MOUNT:
			if(g_st.input_st.plug_usb)
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_EARBUD; //neil 181115 : SWT-595
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_CLICK:
			if(g_st.input_st.plug_usb) //usb plugged - re enter this state
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_3S_HOLD:
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;

		case EVENT_RECEIVE_ACK:
			// both mount, both ack
			if( (g_st.input_st.flag_earbud_got_ack & g_st.input_st.flag_earbud_mount) == 0x03 )
			{
				uart_log("got pair ack2 from both");
				return STATE_IDLE;
			}

			if( g_st.input_st.flag_earbud_got_ack & BIT(e_LEFT) )
			{
				uart_log("got pair ack2 from left");
			}
			if( g_st.input_st.flag_earbud_got_ack & BIT(e_RIGHT) )
			{
				uart_log("got pair ack2 from right");
			}
			return NO_STATE_CHANGE;

		case EVENT_STATE_TIMEOUT:
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_pair_wait2(event_t event)
{
	uart_log("state_exit_pair_wait2");

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}


// -------------------------------------------------------------
state_t state_entry_pdl_reset_wait1(event_t event)
{
	uart_log("state_entry_pdl_reset_wait1");

	send_one_wire_command(e_BOTH, e_CMD_PDL_RESET);
	set_state_timeout(300000); // timeout=5min
	set_led_action_by_state(STATE_PDL_RESET_WAIT_ACK1);

	clear_cradle_ack(e_BOTH);

	return NO_STATE_CHANGE;
}

state_t state_action_pdl_reset_wait1(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			return STATE_CRADLE_CHARGING;

		case EVENT_USB_UNPLUG:
			return STATE_IDLE;

		case EVENT_EARBUD_MOUNT:
			if(g_st.input_st.plug_usb)
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_EARBUD; //neil 181115 : SWT-595
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_CLICK:
			if(g_st.input_st.plug_usb) //usb plugged - re enter this state
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_3S_HOLD:
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;

		case EVENT_RECEIVE_ACK:
			// both mount, both ack
			if( (g_st.input_st.flag_earbud_got_ack & g_st.input_st.flag_earbud_mount) == 0x03 )
			{
				uart_log("got pdl ack1 from both");
				return STATE_PDL_RESET_WAIT_ACK2;
			}

			if( g_st.input_st.flag_earbud_got_ack & BIT(e_LEFT) )
			{
				uart_log("got pdl ack1 from left");
			}
			if( g_st.input_st.flag_earbud_got_ack & BIT(e_RIGHT) )
			{
				uart_log("got pdl ack1 from right");
			}
			return NO_STATE_CHANGE;

		case EVENT_STATE_TIMEOUT:
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_pdl_reset_wait1(event_t event)
{
	uart_log("state_exit_pdl_reset_wait1");

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}


// -------------------------------------------------------------
state_t state_entry_pdl_reset_wait2(event_t event)
{
	uart_log("state_entry_pdl_reset_wait2");

	set_state_timeout(300000); // timeout=5min
	set_led_action_by_state(STATE_PDL_RESET_WAIT_ACK2);

	clear_cradle_ack(e_BOTH);

	return NO_STATE_CHANGE;
}

state_t state_action_pdl_reset_wait2(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			return STATE_CRADLE_CHARGING;

		case EVENT_USB_UNPLUG:
			return STATE_IDLE;

		case EVENT_EARBUD_MOUNT:
			if(g_st.input_st.plug_usb)
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_EARBUD; //neil 181115 : SWT-595
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_CLICK:
			if(g_st.input_st.plug_usb) //usb plugged - re enter this state
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_3S_HOLD:
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;

		case EVENT_RECEIVE_ACK:
			// both mount, both ack
			if( (g_st.input_st.flag_earbud_got_ack & g_st.input_st.flag_earbud_mount) == 0x03 )
			{
				uart_log("got pdl ack2 from both");
				return STATE_IDLE;
			}

			if( g_st.input_st.flag_earbud_got_ack & BIT(e_LEFT) )
			{
				uart_log("got pdl ack2 from left");
			}
			if( g_st.input_st.flag_earbud_got_ack & BIT(e_RIGHT) )
			{
				uart_log("got pdl ack2 from right");
			}
			return NO_STATE_CHANGE;

		case EVENT_STATE_TIMEOUT:
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_pdl_reset_wait2(event_t event)
{
	uart_log("state_exit_pdl_reset_wait2");

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}


// -------------------------------------------------------------
state_t state_entry_power_off_wait(event_t event)
{
	uart_log("state_entry_power_off_wait");

	// HW schematic will be changed.
	// before: 2 step control
	// - FET on/off
	// - DCDC on/off
	// after: 1 step control
	// - DCDC on/off

	//if both are charged more than 95%
	if( g_st.input_st.flag_earbud_charged == g_st.input_st.flag_earbud_mount )
	{
	// 1. charge off
		E_POW_ON_set_level(false);
		uart_log("  Power OFF");

	// 2. wait some time for earbud to wake up
		_delay_ms(50);

	// 3. send 'power off' command to earbud
		if( g_st.input_st.flag_earbud_got_ack & BIT(e_LEFT) )
		{
			send_one_wire_command(e_LEFT, e_CMD_POWER_OFF);
		}
		if( g_st.input_st.flag_earbud_got_ack & BIT(e_RIGHT) )
		{
			send_one_wire_command(e_RIGHT, e_CMD_POWER_OFF);
		}
		clear_cradle_ack(e_BOTH);

	// 4. misc
		set_state_timeout(10000); // timeout=10sec
		set_led_action_by_state(STATE_POWER_OFF_WAIT_ACK);
	}

	return NO_STATE_CHANGE;
}

state_t state_action_power_off_wait(event_t event)
{
	switch(event)
	{
		case EVENT_USB_PLUG:
			return STATE_CRADLE_CHARGING;

		case EVENT_USB_UNPLUG:
			return STATE_IDLE;

		case EVENT_EARBUD_MOUNT:
			if(g_st.input_st.plug_usb)
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_EARBUD;
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_CLICK:
			if(g_st.input_st.plug_usb)
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				g_st.flag_target_blink_led = e_WHICH_LED_BOTH;
				return STATE_BATT_DISPLAY;
			}

		case EVENT_BUTTON_3S_HOLD:
			return STATE_PAIR_WAIT_ACK1;

		case EVENT_BUTTON_10S_HOLD:
			return STATE_PDL_RESET_WAIT_ACK1;

		case EVENT_RECEIVE_ACK:
			return STATE_IDLE;

		case EVENT_STATE_TIMEOUT:
			// check if start charging
			if(USB_INSERT_get_level())
			{
				return STATE_CRADLE_CHARGING;
			}
			else
			{
				return STATE_IDLE;
			}

		case EVENT_STATE_POLL:
			return NO_STATE_CHANGE;

		default:
			return NO_STATE_CHANGE;
	}

	return NO_STATE_CHANGE;
}

void state_exit_power_off_wait(event_t event)
{
	uart_log("state_exit_power_off_wait");
	g_st.one_wire_st[e_LEFT].event = EVENT_NULL;
	g_st.one_wire_st[e_RIGHT].event = EVENT_NULL;
	g_st.input_st.flag_earbud_got_ack = 0x00;

	set_led_action(e_WHICH_LED_BOTH, e_LED_STYLE_POWER_OFF);
}


// -------------------------------------------------------------
void SM_init()
{
	g_st.current_state = STATE_IDLE;
	SM_event_process(EVENT_STATE_POLL); //EVENT_BUTTON_CLICK
}

void SM_event_process(event_t event)
{
	state_t new_state;

	if(state_table[g_st.current_state].action_function == NULL)
	{
		// never come here
		SM_init();
		return;
	}

	new_state = state_table[g_st.current_state].action_function(event);

	while(new_state != NO_STATE_CHANGE)
	{
		g_st.state_tstamp = get_tick_1ms();
		if(state_table[g_st.current_state].exit_function != NULL)
		{
			state_table[g_st.current_state].exit_function(event);
		}

		g_st.current_state = new_state;
		new_state = NO_STATE_CHANGE;
		if(state_table[g_st.current_state].entry_function != NULL)
		{
			new_state = state_table[g_st.current_state].entry_function(event);
		}
	}
}


void poll_button(void)
{
	static uint32_t tstamp_button;

	if(since_1ms(tstamp_button) >= 10)
	{
		tstamp_button = get_tick_1ms();

		if(FUNC_BTN_get_level()==true) // HIGH = pressed
		{
			g_st.button_st.active_cnt++;
			if(g_st.button_st.active_cnt==1000)
			{
				g_st.button_st.event = EVENT_BUTTON_10S_HOLD;
				g_st.button_st.event_tstmp = get_tick_1ms();
			}
			else if(g_st.button_st.active_cnt==300)
			{
				g_st.button_st.event = EVENT_BUTTON_3S_HOLD;
				g_st.button_st.event_tstmp = get_tick_1ms();
			}
		}
		else // low = released
		{
			// released less than 1 sec = single tap
			if( g_st.button_st.is_pressed && g_st.button_st.active_cnt<100) // event - single tap
			{
				g_st.button_st.event = EVENT_BUTTON_CLICK;
				g_st.button_st.event_tstmp = get_tick_1ms();
			}

			g_st.button_st.active_cnt = 0;
			g_st.button_st.is_pressed = false;
		}

		if(g_st.button_st.is_pressed == false && g_st.button_st.active_cnt >= 2) // de-chattering
		{
			g_st.button_st.active_cnt = 2;
			g_st.button_st.is_pressed = true;

			// neil 20181203: SWT-610
			// if button pressed, start charge to power earbuds on to let them ready for any command
			auto_earbud_power();
		}
	}
}


void poll_1wire_input(void)
{
	static uint32_t tstamp_input;
	uint8_t i, st;

	if(since_1ms(tstamp_input) >= 5) //10
	{
		tstamp_input = get_tick_1ms();

		// neil 20181121 - SWT-602
		// check ACK from both sides
		for(i=0;i<2;i++)// 0:left, 1:earbud
		{
			if(i==0)
				st = EL_CONTROL_get_level();
			else
				st = ER_CONTROL_get_level();

			if(st == false) // low = signal
			{
				g_st.one_wire_st[i].active_cnt++;
/*
				if(g_st.one_wire_st[i].active_cnt==200) // 100
				{
					g_st.one_wire_st[i].event = EVENT_BUTTON_1S_HOLD;
					g_st.one_wire_st[i].event_tstmp = get_tick_1ms();
				}
*/
			}
			else // high = idle
			{
				// released less than 1 sec = single tap
				if( g_st.one_wire_st[i].is_pressed && 	/*g_st.input_st.mount_earbud[i] &&*/ g_st.one_wire_st[i].active_cnt<200 ) // 100
				{
					set_cradle_ack(i);
					g_st.one_wire_st[i].event_tstmp = get_tick_1ms();
				}

				g_st.one_wire_st[i].active_cnt = 0;
				g_st.one_wire_st[i].is_pressed = false;
			}

			if(g_st.one_wire_st[i].is_pressed == false && g_st.one_wire_st[i].active_cnt >= 4) // 2
			{
				g_st.one_wire_st[i].active_cnt = 4; // 2
				g_st.one_wire_st[i].is_pressed = true;
			}
		}
	}
}

void poll_port_input(void)
{
	// mount
	if(EL_INSERT_get_level()) // H = not mount
	{
		g_st.input_st.mount_earbud[e_LEFT] = false;
		cbi(g_st.input_st.flag_earbud_mount, 0);
	}
	else // L = mount
	{
		g_st.input_st.mount_earbud[e_LEFT] = true;
		sbi(g_st.input_st.flag_earbud_mount, 0);
	}

	if(ER_INSERT_get_level()) // H = not mount
	{
		g_st.input_st.mount_earbud[e_RIGHT] = false;
		cbi(g_st.input_st.flag_earbud_mount, 1);
	}
	else // L = mount
	{
		g_st.input_st.mount_earbud[e_RIGHT] = true;
		sbi(g_st.input_st.flag_earbud_mount, 1);
	}

	g_st.input_st.flag_earbud_mount &= 0b0011;

	// charging - usb plugged
	g_st.input_st.plug_usb = (USB_INSERT_get_level())? true: false;
}

#define ADC_RPT 2
#define AVR_CNT 10
void poll_adc(bool reset)
{
	uint8_t i;
	static uint32_t adc_cradle10=0l, adc_earbud10[2]={0l,0l};

	g_st.adc_tstamp = get_tick_1ms();

	for(i=0;i<ADC_RPT;i++){ g_st.input_st.adc_cradle = ADC_0_get_conversion(ADC_CH1_CRADLE_BATT); _delay_us(10);}
	for(i=0;i<ADC_RPT;i++){ g_st.input_st.adc_earbud[e_LEFT] = ADC_0_get_conversion(ADC_CH3_EARBUD_L_BATT); _delay_us(10);}
	for(i=0;i<ADC_RPT;i++){ g_st.input_st.adc_earbud[e_RIGHT] = ADC_0_get_conversion(ADC_CH2_EARBUD_R_BATT); _delay_us(10);}

	if(reset)
	{
		// regard 1st sample as average
		adc_cradle10 = g_st.input_st.adc_cradle*10;
		adc_earbud10[e_LEFT] = g_st.input_st.adc_earbud[e_LEFT]*10;
		adc_earbud10[e_RIGHT] = g_st.input_st.adc_earbud[e_RIGHT]*10;
	}
	else
	{
		// calculate average - no float: multiply 10 for accuracy
		adc_cradle10 = (adc_cradle10*(AVR_CNT-1) + g_st.input_st.adc_cradle*10) / AVR_CNT;
		adc_earbud10[e_LEFT] = (adc_earbud10[e_LEFT]*(AVR_CNT-1) + g_st.input_st.adc_earbud[e_LEFT]*10) / AVR_CNT;
		adc_earbud10[e_RIGHT] = (adc_earbud10[e_RIGHT]*(AVR_CNT-1) + g_st.input_st.adc_earbud[e_RIGHT]*10) / AVR_CNT;
	}

	g_st.input_st.adc_cradle_avr = (adc_cradle10+5)/10;
	g_st.input_st.adc_earbud_avr[e_LEFT] = (adc_earbud10[0]+5)/10;
	g_st.input_st.adc_earbud_avr[e_RIGHT] = (adc_earbud10[1]+5)/10;
}


// extract events
// send events to state machine
// update status
void discover_state_event(void)
{
	uint8_t i;
	uint16_t limit1, limit2;

	// update earbud mount info ------------------------------------------
//	if(   (g_st.prev_input_st.mount_earbud[e_LEFT] != g_st.input_st.mount_earbud[e_LEFT]) ||
//		(g_st.prev_input_st.mount_earbud[e_RIGHT] != g_st.input_st.mount_earbud[e_RIGHT] ) )
	if( g_st.prev_input_st.flag_earbud_mount != g_st.input_st.flag_earbud_mount )
	{
		auto_earbud_power();

		// neil 181113 : SWT-587
		// wait 1.5 sec for earbud to start charge
		// blocking delay - bad habit but to make it simple...
		// todo: non-blocking
		if(g_st.input_st.mount_earbud[e_LEFT] || g_st.input_st.mount_earbud[e_RIGHT])
		{
			wait_earbud_charger_ms(1500);
		}

		SM_event_process(EVENT_EARBUD_MOUNT);
	}

	// update usb charger plug info ---------------------------------------
	if( g_st.prev_input_st.plug_usb != g_st.input_st.plug_usb )
	{
		if(g_st.input_st.plug_usb)
		{
			SM_event_process(EVENT_USB_PLUG);
		}
		else
		{
			SM_event_process(EVENT_USB_UNPLUG);
		}
	}

	// update earbud battery level ----------------------------------------
	for(i=0; i<2; i++)
	{
		if(g_st.input_st.mount_earbud[i])
		{
			// getting lower - apply hystresis - SWT-573
			// lower adc value means higher battery level
			if( g_st.input_st.adc_earbud_avr[i] <= EARBUD_BATT_LEVEL_95P)
			{
				g_st.input_st.batt_level_earbud[i] = e_EARBUD_BATT_HIGH;
			}
			else if( g_st.input_st.adc_earbud_avr[i] > (EARBUD_BATT_LEVEL_95P + EARBUD_BATT_LEVEL_HYSTERESIS) )
			{
				g_st.input_st.batt_level_earbud[i] = e_EARBUD_BATT_LOW;
			}
			else
			{
				// keep previous color grade
				g_st.input_st.batt_level_earbud[i] = g_st.prev_input_st.batt_level_earbud[i];
			}
		}
		else
		{
			g_st.input_st.batt_level_earbud[i] = e_EARBUD_BATT_INVALID;
		}

		if( g_st.input_st.batt_level_earbud[i] == e_EARBUD_BATT_HIGH )
			sbi( g_st.input_st.flag_earbud_charged, i);
		else
			cbi( g_st.input_st.flag_earbud_charged, i);
	}


	// update cradle battery level ----------------------------------------
	// limit for battery level - charging(usb plugged)
	if(g_st.input_st.plug_usb)
	{
		limit1 = CRADLE_BATT_LEVEL1_CHARGING;
		limit2 = CRADLE_BATT_LEVEL2_CHARGING;
	}
	// limit for battery level - discharging(usb unplugged)
	else
	{
		limit1 = CRADLE_BATT_LEVEL1_DISCHARGING;
		limit2 = CRADLE_BATT_LEVEL2_DISCHARGING;
	}

	// apply hysteresis - SWT-573, SWT-564
	if( g_st.input_st.adc_cradle >= limit2)
	{
		g_st.input_st.batt_level_cradle = e_CRADLE_BATT_HIGH;
	}
	else if( 	(g_st.input_st.adc_cradle >= limit1) &&
			g_st.input_st.adc_cradle < (limit2 - CRADLE_BATT_LEVEL_HYSTERESIS))
	{
		g_st.input_st.batt_level_cradle = e_CRADLE_BATT_MEDIUM;
	}
	else if(g_st.input_st.adc_cradle < (limit1 - CRADLE_BATT_LEVEL_HYSTERESIS))
	{
		g_st.input_st.batt_level_cradle = e_CRADLE_BATT_LOW;
	}
	else
	{
		// keep previous color grade
		g_st.input_st.batt_level_cradle = g_st.prev_input_st.batt_level_cradle;
	}


	// button event ------------------------------------------------------
	switch(g_st.button_st.event)
	{
		// todo: add events
		//	case EVENT_BUTTON_3S_HOLD_READY:
		//	case EVENT_BUTTON_10S_HOLD_READY:
		case EVENT_BUTTON_CLICK:
		case EVENT_BUTTON_3S_HOLD:
		case EVENT_BUTTON_10S_HOLD:
		case EVENT_NULL:
			SM_event_process(g_st.button_st.event);
			g_st.button_st.event = EVENT_NULL;
			break;
		default:
			g_st.button_st.event = EVENT_NULL;
			break;
	}

	// ack from 1-wire ----------------------------------------------------
	// neil 20181121 - SWT-602
	if( g_st.input_st.flag_earbud_got_ack ) // from any
	{
		SM_event_process(EVENT_RECEIVE_ACK);
	}

	// state timeout ----------------------------------------------------
	if( g_st.state_timeout>0 && // 0 means no timeout
		(!g_st.button_st.is_pressed || g_st.button_st.event != EVENT_NULL) )
	{
		if( since_1ms(g_st.state_tstamp) >= g_st.state_timeout)
		{
			g_st.state_tstamp = get_tick_1ms();
			SM_event_process(EVENT_STATE_TIMEOUT);
			uart_log("state timeout");
		}
	}

	// update status ------------------------------------------------------
	memcpy( &g_st.prev_input_st, &g_st.input_st, sizeof(st_port_input_t) );
}


void poll_status(void)
{
	poll_port_input();
	poll_button();
	poll_1wire_input();

	// adc polling with 100ms period
	if(since_1ms(g_st.adc_tstamp)>100)
	{
		poll_adc(false);
		wdt_reset(); //neil 181112 : SWT-592

#ifdef DEBUG_ON
		{
			static uint16_t cnt;
			cnt++;
			if(cnt%10==0) print_adc();
		}
#endif
	}

	discover_state_event();
}


void task_led(void)
{
	uint8_t idx;
	uint8_t sel_idx;

	for(sel_idx=0xff, idx=0; idx<2; idx++)
	{
		if(g_st.led_st[idx].action == e_LED_STYLE_BLINK)
		{
			sel_idx = idx;
		}
	}

	// blink - use blink control info for selected one - for synchronous blinking
	if(sel_idx != 0xff)
	{
		if(since_1ms(g_st.led_st[sel_idx].blink_tsamp) >= g_st.led_st[sel_idx].blink_period)
		{
			g_st.led_st[sel_idx].blink_tsamp = get_tick_1ms();
			g_st.led_st[sel_idx].blink_index ^= 1;

			for(idx=0; idx<2; idx++)
			{
				if(g_st.led_st[idx].action == e_LED_STYLE_BLINK)
				{
					g_st.led_st[idx].blink_tsamp = g_st.led_st[sel_idx].blink_tsamp;
					g_st.led_st[idx].blink_index = g_st.led_st[sel_idx].blink_index;

					I2C_set_color(idx, g_st.led_st[idx].blink_color[ g_st.led_st[idx].blink_index ]);
				}
			}
		}
	}

	// 0:e_WHICH_LED_CRADLE, 1:e_WHICH_LED_EARBUD
	for(idx=0; idx<2; idx++)
	{
		// solid on / off
		if(g_st.led_st[idx].action == e_LED_STYLE_ON_SOLID)
		{
			I2C_set_color(idx, g_st.led_st[idx].solid_color);
		}

		// led power off
		else if(g_st.led_st[idx].action == e_LED_STYLE_POWER_OFF)
		{
			I2C_set_color(idx, e_LED_COLOR_OFF);
		}
	}
}


void task_states(void)
{
	SM_event_process(EVENT_STATE_POLL);
}


