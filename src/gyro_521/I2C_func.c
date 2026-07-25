#include "stm32.h"
#include "i2c.h"
#include <zephyr/kernel.h>
#include <stdlib.h>

#define MSB  7
#define BYTE 8
#define BIT_POSITION(bit) (MSB - (bit % BYTE))
#define KHZ(freq)         (freq*1000)
static char i2c_send(unsigned char *data_buffer, unsigned short byte_count, const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{
	unsigned short bit_index = 0;

	while(bit_index < (byte_count*8))
	{
		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);
		k_usleep(delay);

		if(*(data_buffer) & (1 << BIT_POSITION(bit_index)))
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
		else
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_LOW);

		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
		if((BIT_POSITION(bit_index) == MSB))
			while(!gpio_input_read(i2c_bus_details->scl_port, i2c_bus_details->scl_pin))
		k_usleep(delay);

		bit_index++;

		if((bit_index % BYTE) == 0)
		{
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
			k_usleep(delay);
			if(gpio_input_read(i2c_bus_details->sda_port, i2c_bus_details->sda_pin))
				while(!gpio_input_read(i2c_bus_details->scl_port, i2c_bus_details->scl_pin))
		        data_buffer++;
		}
		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);
		k_usleep(delay);

	}
	return RETURN_SUCCESS;
}

static char i2c_receive(unsigned char *data_buffer, unsigned short byte_count, const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{
	unsigned short bit_index   = 0;
	while(bit_index < (byte_count*8))
	{
		*(data_buffer) = 0;

		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
		if((BIT_POSITION(bit_index) == MSB))
			while(!gpio_input_read(i2c_bus_details->scl_port, i2c_bus_details->scl_pin))
		k_usleep(delay);

		if(gpio_input_read(i2c_bus_details->sda_port, i2c_bus_details->sda_pin))
			*(data_buffer) |= (1 << BIT_POSITION(bit_index));

		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);
		k_usleep(delay);
		bit_index++;

		if((bit_index % BYTE) == 0)
		{
			if(bit_index == (byte_count*8))
				gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH); 
			else
				gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_LOW); 

			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);     
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);      
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH); 
			data_buffer++;
		}
	}
	return RETURN_SUCCESS;

}

void stop_condition(const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{
	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW); 
	k_usleep(delay);
	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_LOW);
	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
	k_usleep(delay);
	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
}

void start_condition(const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{
	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
	k_usleep(delay);
	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_LOW);
}

struct i2c_bus_details *i2c_bus_configure(struct i2c_bus_details *i2c_bus_details)
{
	struct i2c_bus_details *configured_bus;
	if(!i2c_bus_details)
		return NULL;

	gpio_output_type(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, OPEN_DRAIN);
	gpio_pull_set   (i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PULL_UP);
	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
	gpio_mode_set   (i2c_bus_details->sda_port, i2c_bus_details->sda_pin, OUTPUT_MODE);

	gpio_output_type(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, OPEN_DRAIN);
	gpio_pull_set   (i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PULL_UP);
	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
	gpio_mode_set   (i2c_bus_details->scl_port, i2c_bus_details->scl_pin, OUTPUT_MODE);

	configured_bus = malloc(sizeof(struct i2c_bus_details));

	if(!configured_bus)
		return NULL;      
	*configured_bus = *i2c_bus_details;

	return configured_bus;
}

char i2c_transfer(struct i2c_target_details *i2c_target_details, struct i2c_transfer_details *i2c_write_details, struct i2c_transfer_details *i2c_read_details)
{
	unsigned char delay, address_byte;
	char ret;
	if((!i2c_target_details) || (i2c_target_details->target_address > 127) || !(i2c_target_details->bus_details))
		return -INVARG;
	if(i2c_write_details)
	{
		if(!i2c_write_details->data_buffer || !i2c_write_details->byte_count)
			return -INVARG;
	}

	if(i2c_read_details)
	{
		if(!i2c_read_details->data_buffer || !i2c_read_details->byte_count)
			return -INVARG;
	}


	delay = (1000000 / KHZ(i2c_target_details->mode)) / 2;

	start_condition(i2c_target_details->bus_details, delay);
	address_byte = (i2c_target_details->target_address << 1) |((i2c_read_details && !i2c_write_details) ? I2C_READ : I2C_WRITE);

	ret = i2c_send(&address_byte, 1 , i2c_target_details->bus_details, delay);

	if(ret || (!i2c_write_details && !i2c_read_details))
		goto stop;

	if(i2c_write_details)
	{
		ret = i2c_send(i2c_write_details->data_buffer, 
				i2c_write_details->byte_count, 
				i2c_target_details->bus_details, delay);
		if(ret)
				goto stop;
	}

	if(i2c_read_details)
	{
		if(i2c_write_details)
		{
			start_condition(i2c_target_details->bus_details, delay);
			address_byte = (i2c_target_details->target_address << 1) | I2C_READ;
			ret = i2c_send(&address_byte, 1 , i2c_target_details->bus_details, delay);
			if(ret)
				goto stop;
		}
		ret = i2c_receive(i2c_read_details->data_buffer, 
				i2c_read_details->byte_count, 
				i2c_target_details->bus_details, delay);
	}
stop:
	stop_condition(i2c_target_details->bus_details, delay);
	return ret;
}

