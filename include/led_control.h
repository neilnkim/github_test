/*
 * led_control.h
 *
 * Created: 2018-06-10 오후 3:54:01
 *  Author: user
 */


#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_

#include "atmel_start.h"
#include "app_config.h"


#define CRADLE_LED_CERRENT		10
#define EARBUD_LED_CERRENT	10

// i2c device address
#define I2C_ADDR_EARBUD_LED 0x30
#define I2C_ADDR_CRADLE_LED 0x32

// led id
#define ID_CRADLE_LED	0
#define ID_EARBUD_LED	1

// LP5562(LED IC) Register address
#define	LP5562_REG_ENABLE				0x00
#define	LP5562_REG_OP_MODE				0x01
#define	LP5562_REG_B_PWM				0x02
#define	LP5562_REG_G_PWM				0x03
#define	LP5562_REG_R_PWM				0x04
#define	LP5562_REG_B_CURRENT			0x05
#define	LP5562_REG_G_CURRENT			0x06
#define	LP5562_REG_R_CURRENT			0x07
#define	LP5562_REG_CONFIG				0x08
#define	LP5562_REG_ENG1_PC				0x09
#define	LP5562_REG_ENG2_PC				0x0a
#define	LP5562_REG_ENG3_PC				0x0b
#define	LP5562_REG_STATUS				0x0c
#define	LP5562_REG_RESET				0x0d
#define	LP5562_REG_W_PWM				0x0e
#define	LP5562_REG_W_CURRENT			0x0f
#define	LP5562_REG_LED_MAP				0x70


uint8_t I2C_led_power_on(bool led_id);
uint8_t I2C_led_power_off(bool led_id);
uint8_t I2C_set_intensity(bool led_id, uint8_t r_value, uint8_t g_value, uint8_t b_value);
uint8_t I2C_set_rgb(bool led_id, uint8_t red_color, uint8_t green_color, uint8_t blue_color);
uint8_t I2C_set_color(bool led_id, uint8_t color_name);


#endif /* LED_CONTROL_H_ */
