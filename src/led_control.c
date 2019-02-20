/*
 * LED_control.c
 *
 * Created: 2018-06-10 오후 2:24:44
 *  Author: kevin.ko
 */
 #include <atmel_start.h>
 #include <atomic.h>
 #include <util/delay.h>
 #include "app_config.h"
 #include "led_control.h"


 /** Structure passed into read_handler to describe the actions to be performed by the handler */
 typedef struct
 {
	 uint8_t *data;
	 uint8_t  size;
 } transfer_descriptor_t;


void I2C_do_write_transfer(i2c_address_t slave_adr, uint8_t * w_data, uint8_t size)
{
	uint8_t fail_count = 0;
	while (!I2C_0_open(slave_adr))
	{
		if(++fail_count == 3)
		{
			i2c_cb_return_stop(NULL);
			return;
		}
	}
	//; // sit here until we get the bus..
	fail_count = 0;
	I2C_0_set_address_nack_callback(i2c_cb_return_stop, NULL);
	I2C_0_set_buffer((void *)w_data, 2);
	I2C_0_master_operation(false);
	while (I2C_BUSY == I2C_0_close())
	; // sit here until the entire chained operation has finished
	/*while (I2C_BUSY == I2C_0_close())
	{
		if(++fail_count == 100)
		{
			return;
		}
	}*/
}

/** This callback is called when the initial write of the pointer register has finished.
    This callback controls the second phase of the I2C transaction, the read of the
    targeted register after a REPEATED START.
*/
i2c_operations_t I2C_0_read_handler(void *d)
{
	transfer_descriptor_t *desc = (transfer_descriptor_t *)d;
	I2C_0_set_buffer((void *)desc->data, desc->size);
	// Set callback to terminate transfer and send STOP after read is complete
	I2C_0_set_data_complete_callback(i2c_cb_return_stop, NULL);

	return i2c_restart_read; // Send REPEATED START before read
}



uint8_t I2C_led_power_on(bool led_id)
{
	uint8_t i2c_addr = ((led_id == ID_EARBUD_LED)? I2C_ADDR_EARBUD_LED: I2C_ADDR_CRADLE_LED);
	uint8_t write_data[2];

	g_st.led_st[led_id].power = true;

	// Chip enable
	write_data[0]= LP5562_REG_ENABLE; // Enable Reg.
	write_data[1]= 0x40;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	// Enable internal clock
	write_data[0]= LP5562_REG_CONFIG;	// Config Reg.
	write_data[1]= 0x21;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	// Configure all LED outputs to be controlled from I2C registers
	write_data[0]= LP5562_REG_LED_MAP;	// Config Reg.
	write_data[1]= 0x00;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	return 0;
}

uint8_t I2C_led_power_off(bool led_id)
{
	uint8_t i2c_addr = ((led_id == ID_EARBUD_LED)? I2C_ADDR_EARBUD_LED: I2C_ADDR_CRADLE_LED);
	uint8_t write_data[2];

	g_st.led_st[led_id].power = false;

	// Enable internal clock
	write_data[0]= LP5562_REG_CONFIG;	// Config Reg.
	write_data[1]= 0x21;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	// LED power off
	write_data[0]= LP5562_REG_ENABLE; // Enable Reg.
	write_data[1]= 0x00;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	return 0;
}

uint8_t I2C_set_intensity(bool led_id, uint8_t r_value, uint8_t g_value, uint8_t b_value)
{
	uint8_t i2c_addr = ((led_id == ID_EARBUD_LED)? I2C_ADDR_EARBUD_LED: I2C_ADDR_CRADLE_LED);
	uint8_t write_data[2];

	if(g_st.led_st[led_id].power==false)
		I2C_led_power_on(led_id);

	write_data[0]= LP5562_REG_R_CURRENT; // Enable Reg.
	write_data[1]= r_value;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	write_data[0]= LP5562_REG_G_CURRENT; // Enable Reg.
	write_data[1]= g_value;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	write_data[0]= LP5562_REG_B_CURRENT; // Enable Reg.
	write_data[1]= b_value;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	return 0;
}

uint8_t I2C_set_rgb(bool led_id, uint8_t red_color, uint8_t green_color, uint8_t blue_color)
{
	uint8_t i2c_addr = ((led_id == ID_EARBUD_LED)? I2C_ADDR_EARBUD_LED: I2C_ADDR_CRADLE_LED);
	uint8_t write_data[2];

	if(g_st.led_st[led_id].power==false)
		I2C_led_power_on(led_id);

	write_data[0]= LP5562_REG_R_PWM;
	write_data[1]= red_color;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	write_data[0]= LP5562_REG_G_PWM;
	write_data[1]= green_color;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	write_data[0]= LP5562_REG_B_PWM;
	write_data[1]= blue_color;
	I2C_do_write_transfer(i2c_addr, write_data, 2);

	return 0;
}

uint8_t I2C_set_color(bool led_id, uint8_t color_name)
{
	uint8_t r, g, b;

	switch(color_name)
	{
		case e_LED_COLOR_OFF:
			r=COLOR_OFF_R; g=COLOR_OFF_G; b=COLOR_OFF_B;
			break;
		case e_LED_COLOR_RED:
			r=COLOR_RED_R; g=COLOR_RED_G; b=COLOR_RED_B;
			break;
		case e_LED_COLOR_GREEN:
			r=COLOR_GREEN_R; g=COLOR_GREEN_G; b=COLOR_GREEN_B;
			break;
		case e_LED_COLOR_ORANGE:
			r=COLOR_ORANGE_R; g=COLOR_ORANGE_G; b=COLOR_ORANGE_B;
			break;
		case e_LED_COLOR_WHITE:
			r=COLOR_WHITE_R; g=COLOR_WHITE_G; b=COLOR_WHITE_B; // warm white
			break;
		default:
			r=255; g=255; b=255; // white
			break;
	}

	if(g_st.led_st[led_id].power==false)
		I2C_led_power_on(led_id);

	I2C_set_rgb(led_id, r, g, b);

	return 0;
}


