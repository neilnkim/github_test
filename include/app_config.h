/*
 * app_common.h
 *
 * Created: 2018-06-10 AM 11:35:35
 *  Author: kevin.ko
 */


#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_
#include "atmel_start.h"
#include <stdio.h>

//#define DEBUG_ON

// F/W Version ---------------------
// v0.8.0 2018/10/05 - neil
// v0.9.0 2018/11/01 - neil
// v0.10.0 2018/11/08 - neil
// v0.11.0 2018/11/16 - neil
// v0.12.0 2018/11/23 - neil
// v0.13.0 2018/12/14 - neil
//----------------------------------
#define FW_VER "0.13.0"
#define DATE_RELEASE "2018/12/14"
//----------------------------------

// UART Log
#ifdef DEBUG_ON
	#define uart_log(msg...) {printf(msg); printf("\r\n");}
#else
	#define uart_log(msg...)
#endif

#define UINT32_T_MAX (0xffffffff)
#define BIT(x) (1<<(x))
#define cbi(reg, bit) reg &= ~(BIT(bit))
#define sbi(reg, bit) reg |= (BIT(bit))

/* -------------------------------
	PA1 / ADC / cradle battery
	PA2 / ADC / earbud R
	PA3 / ADC / earbud L
	PA4 / IO / 1-wire L - signal=LOW, idle=HiZ
	PA5 / IO / 1-wire R - signal=LOW, idle=HiZ
	PA6 / IN / earbud mount R - mount=LOW
	PA7 / IN / earbud mount L - mount=LOW
	PB0 / I2C / SCL
	PB1 / I2C / SDA
	PB2 / UART / TX
	PB3 / UART / RX
	BP4 / IN / usb plug - plug=HIGH
	PB5 / IN / button - push=HIGH
	PC0 / OUT / earbud power R
	PC1 / OUT / earbud power L
	PC2 / OUT / power block
	PC3 / IN / charge status
------------------------------- */

#define ADC_CH1_CRADLE_BATT		1
#define ADC_CH2_EARBUD_R_BATT	2
#define ADC_CH3_EARBUD_L_BATT	3

#define COLOR_WHITE_R 255
#define COLOR_WHITE_G 150
#define COLOR_WHITE_B 50
#define COLOR_RED_R 255
#define COLOR_RED_G 0
#define COLOR_RED_B 0
#define COLOR_GREEN_R 30
#define COLOR_GREEN_G 255
#define COLOR_GREEN_B 26
#define COLOR_ORANGE_R 255
#define COLOR_ORANGE_G 65
#define COLOR_ORANGE_B 0
#define COLOR_OFF_R 0
#define COLOR_OFF_G 0
#define COLOR_OFF_B 0


// new definition (Vdd=2.1V)
// neil 181030 SWT-570
// cradle battery level (charging)
// 3 step: RED / ORANGE / GREEN
#define CRADLE_BATT_LEVEL0_CHARGING		487 // 3.00~3.60V(ADC: 487~585)	//RED
#define CRADLE_BATT_LEVEL1_CHARGING		586 // 3.61~4.17V(ADC: 586~674)	//ORANGE
#define CRADLE_BATT_LEVEL2_CHARGING		675 // 4.18~4.20V(ADC: 675~683)	//GREEN

// cradle battery level (discharging)
// 3 step: RED / ORANGE / GREEN
#define CRADLE_BATT_LEVEL0_DISCHARGING	487 // 3.00~3.55V(ADC: 487~577) //RED
#define CRADLE_BATT_LEVEL1_DISCHARGING	578 // 3.56~3.96V(ADC: 578~643) //ORANGE
#define CRADLE_BATT_LEVEL2_DISCHARGING	644	// 3.97~4.20V(ADC: 644~683) //GREEN
#define CRADLE_BATT_LEVEL_HYSTERESIS	5

/* previous definition (Vdd=1.8V)
// cradle battery level (charging)
// 3 step: RED / ORANGE / GREEN
#define CRADLE_BATT_LEVEL0_CHARGING		504 // 3.00~3.60V(ADC: 504~591)	//RED
#define CRADLE_BATT_LEVEL1_CHARGING		592 // 3.61~4.17V(ADC: 592~684)	//ORANGE
#define CRADLE_BATT_LEVEL2_CHARGING		685 // 4.18~4.20V(ADC: 685~689)	//GREEN

// cradle battery level (discharging)
// 3 step: RED / ORANGE / GREEN
#define CRADLE_BATT_LEVEL0_DISCHARGING	504 // 3.00~3.60V(ADC: 504~582) //RED
#define CRADLE_BATT_LEVEL1_DISCHARGING	583 // 3.61~4.17V(ADC: 583~651) //ORANGE
#define CRADLE_BATT_LEVEL2_DISCHARGING	652	// 3.97~4.20V(ADC: 652~689) //GREEN
*/

// earbud battery level
// 2 step: ORANGE / GREEN
#define EARBUD_BATT_LEVEL_95P			69 // 70 // 4.1 V, 95%	GREEN/ORANGE
#define EARBUD_BATT_LEVEL_HYSTERESIS	5


#define BLINK_FAST_MS		33
#define BLINK_MEDIUM_MS		(BLINK_FAST_MS*2.5) //82.5
#define BLINK_SLOW_MS		(BLINK_FAST_MS*4)	//132
#define BLINK_SLOW2_MS		(BLINK_FAST_MS*7.5)	//247.5


enum
{
	e_EARBUD_BATT_LOW = 0,
	e_EARBUD_BATT_HIGH,
	e_EARBUD_BATT_INVALID
};

enum
{
	e_CRADLE_BATT_LOW = 0,
	e_CRADLE_BATT_MEDIUM,
	e_CRADLE_BATT_HIGH,
	e_CRADLE_BATT_INVALID
};

enum
{
	e_LED_STYLE_POWER_OFF = 0,
	e_LED_STYLE_BLINK,
	e_LED_STYLE_ON_SOLID
};

enum
{
	e_LED_COLOR_OFF = 0,
	e_LED_COLOR_RED,
	e_LED_COLOR_GREEN,
	e_LED_COLOR_ORANGE,
	e_LED_COLOR_WHITE
};

enum
{
	e_WHICH_LED_CRADLE = 0,
	e_WHICH_LED_EARBUD,
	e_WHICH_LED_BOTH
};

enum
{
	e_CMD_PAIR = 0,
	e_CMD_PDL_RESET,
	e_CMD_POWER_OFF
};

enum
{
	e_INPUT = 0,
	e_OUTPUT
};

enum
{
	e_LEFT = 0,
	e_RIGHT,
	e_BOTH
};

enum
{
	e_LOW = 0,
	e_HIGH
};


typedef struct
{
	bool 	power;
	uint8_t action; 			// blink, solid
	uint8_t solid_color;
	uint8_t blink_color[2];	// blink - switch 2 blink_color
	uint8_t blink_index; 	// on or off when blinking - o:on, 1:off
	uint32_t blink_period;
	uint32_t blink_tsamp;
}__attribute__ ((packed))st_led_status_t;

typedef struct
{
	bool 	is_pressed;
	uint8_t event;			// click, pre_hold3, pre_hold10, hold3, hold10
	uint8_t click_cnt;
	uint16_t active_cnt;	// count chattering
	uint32_t event_tstmp;
}__attribute__ ((packed))st_button_status_t;

typedef struct
{
	bool plug_usb;
	bool mount_earbud[2]; // 0:left, 1:right

	uint8_t flag_earbud_mount; 	// bit 0:left, 1:right
	uint8_t flag_earbud_charged;	// bit 0:left, 1:right
	uint8_t flag_earbud_got_ack;	// bit 0:left, 1:right

	uint16_t adc_cradle;
	uint16_t adc_cradle_avr;
	uint16_t adc_earbud[2]; // 0:left, 1:right
	uint16_t adc_earbud_avr[2];

	uint8_t batt_level_cradle;
	uint8_t batt_level_earbud[2]; // 0:left, 1:right
}__attribute__ ((packed))st_port_input_t;

typedef struct
{
	uint32_t sys_tick_1ms;
	uint32_t state_tstamp;
	uint32_t state_timeout; // unit=ms, 0=infinit
	uint32_t adc_tstamp;

	uint8_t current_state;
	uint8_t flag_target_blink_led;

	st_led_status_t led_st[2]; // 0:cradle, 1:earbud
	st_button_status_t button_st;
	st_button_status_t one_wire_st[2]; // 0:left, 1:earbud
	st_port_input_t input_st;
	st_port_input_t prev_input_st;
}__attribute__ ((packed))st_status_t;

extern st_status_t	g_st;

#endif /* APP_CONFIG_H_ */
