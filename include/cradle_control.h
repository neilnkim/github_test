/*
 * cradle_control.h
 *
 * Created: 2018-06-11 오전 11:19:09
 *  Author: kevin.ko
 */


#ifndef CRADLE_CONTROL_H_
#define CRADLE_CONTROL_H_

#include "app_config.h"

void sleep_wakeup(void);
void sleep_start(void);

uint32_t get_tick_1ms(void);
uint32_t since_1ms( uint32_t tstamp );

void set_state_timeout( uint32_t timeout_ms );
void update_battery_led_color(void);
void set_led_action(uint8_t idx, uint8_t led_action);
void set_led_action_by_state(uint8_t mode);
void print_battery_level(void);
void auto_earbud_power(void);
void manual_earbud_power(bool on);
void send_one_wire_command(uint8_t earbud, uint8_t cmd);
void set_one_wire_io_mode(uint8_t earbud, uint8_t mode);
void set_one_wire_idle(uint8_t earbud);
void set_one_wire_signal(uint8_t earbud);
void print_adc(void);
void set_port_input_changed(void);
void print_version(void);
void led_show_rebooting(void);
void wait_earbud_charger_ms(uint16_t ms);
void set_cradle_ack(uint8_t idx);
void clear_cradle_ack(uint8_t idx);


#endif /* CRADLE_CONTROL_H_ */
