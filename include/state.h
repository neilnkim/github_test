/*
 * state.h
 *
 * Created: 2018-10-18 PM 12:21:27
 *  Author: neil
 */


#ifndef STATE_H_
#define STATE_H_


typedef enum state_e
{
	STATE_IDLE = 0,
	STATE_CRADLE_CHARGING,
	STATE_BATT_DISPLAY,
	STATE_PAIR_WAIT_ACK1,
	STATE_PAIR_WAIT_ACK2,
	STATE_PDL_RESET_WAIT_ACK1,
	STATE_PDL_RESET_WAIT_ACK2,
	STATE_POWER_OFF_WAIT_ACK,
	NUM_STATES,
	NO_STATE_CHANGE
} state_t;

typedef enum event_e
{
	EVENT_NULL = 0,
	EVENT_USB_PLUG,
	EVENT_USB_UNPLUG,
	EVENT_EARBUD_MOUNT,
//	EVENT_EARBUD_UNMOUNT,
	EVENT_BUTTON_CLICK,
	EVENT_BUTTON_1S_HOLD_READY,
	EVENT_BUTTON_3S_HOLD_READY,
	EVENT_BUTTON_10S_HOLD_READY,
	EVENT_BUTTON_1S_HOLD,
	EVENT_BUTTON_3S_HOLD,
	EVENT_BUTTON_10S_HOLD,
	EVENT_RECEIVE_ACK,
	EVENT_STATE_TIMEOUT,
	EVENT_STATE_POLL,
	NUM_EVENTS
} event_t;

typedef state_t (*state_entry_func_t)(event_t);
typedef state_t (*state_action_func_t)(event_t);
typedef void (*state_exit_func_t)(event_t);

typedef struct state_functions_s
{
	state_entry_func_t entry_function;
	state_action_func_t action_function;
	state_exit_func_t exit_function;
} state_functions_t;

state_t state_entry_idle(event_t event);
state_t state_action_idle(event_t event);
void state_exit_idle(event_t event);
state_t state_entry_cradle_charging(event_t event);
state_t state_action_cradle_charging(event_t event);
void state_exit_cradle_charging(event_t event);
state_t state_entry_batt_display(event_t event);
state_t state_action_batt_display(event_t event);
void state_exit_batt_display(event_t event);
state_t state_entry_pair_wait1(event_t event);
state_t state_action_pair_wait1(event_t event);
void state_exit_pair_wait1(event_t event);
state_t state_entry_pair_wait2(event_t event);
state_t state_action_pair_wait2(event_t event);
void state_exit_pair_wait2(event_t event);
state_t state_entry_pdl_reset_wait1(event_t event);
state_t state_action_pdl_reset_wait1(event_t event);
void state_exit_pdl_reset_wait1(event_t event);
state_t state_entry_pdl_reset_wait2(event_t event);
state_t state_action_pdl_reset_wait2(event_t event);
void state_exit_pdl_reset_wait2(event_t event);
state_t state_entry_power_off_wait(event_t event);
state_t state_action_power_off_wait(event_t event);
void state_exit_power_off_wait(event_t event);


void SM_init();
void SM_event_process(event_t event);
void task_states(void);
void poll_port_input(void);
void poll_adc(bool reset);
void poll_button(void);
void poll_1wire_input(void);
void discover_state_event(void);
void poll_status(void);
void task_led(void);

#endif /* STATE_H_ */
