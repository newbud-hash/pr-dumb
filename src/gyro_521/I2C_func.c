#include "stm32.h"
#include "i2c.h"
#include <zephyr/kernel.h>
#include <stdlib.h>

struct i2c_bus_details *bus_table = NULL;
unsigned char bus_count = 0;

char i2c_send(unsigned char *data_buffer, unsigned short byte_count, const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{
	unsigned short total_bits  = byte_count * 8;
	unsigned short bit_index   = 0;
	while(bit_index < total_bits)
	{
		unsigned short byte_position = bit_index / 8;
		unsigned char  bit_position  = 7 - (bit_index % 8); 
		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);
		k_usleep(delay);

		if(*(data_buffer + byte_position) & (1 << bit_position))
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
		else
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_LOW);
		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
		if((bit_position == 7) && (byte_position > 0))
			while(!gpio_input_read(i2c_bus_details->scl_port, i2c_bus_details->scl_pin));
		k_usleep(delay);
		bit_index++;

		if(byte_position && ((bit_index % 8) == 0))
		{
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
			k_usleep(delay);
			if(gpio_input_read(i2c_bus_details->sda_port, i2c_bus_details->sda_pin))
			{
				gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW); 
				return -RETURN_FAILURE;
			}


			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW); 
		}

		return RETURN_SUCCESS;
	}
}

char i2c_receive(unsigned char *data_buffer, unsigned short byte_count, const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{

	unsigned short total_bits = byte_count * 8;
	unsigned short bit_index  = 0;

	while(bit_index < total_bits)
	{
		unsigned short byte_position = bit_index / 8;
		unsigned char  bit_position  = 7 - (bit_index % 8);

		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
		if((bit_position == 7) && (byte_position > 0))
			while(!gpio_input_read(i2c_bus_details->scl_port, i2c_bus_details->scl_pin));
		k_usleep(delay);

		if(gpio_input_read(i2c_bus_details->sda_port, i2c_bus_details->sda_pin))
			*(data_buffer + byte_position) |= (1 << bit_position);
		else
			*(data_buffer + byte_position) &= ~(1 << bit_position);

		gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);
		k_usleep(delay);
		bit_index++;
		if(byte_position && ((bit_index % 8) == 0))
		{
			if(byte_position == byte_count - 1)
				gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH); 
			else
				gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_LOW); 

			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);     
			k_usleep(delay);
			gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_LOW);      
			gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH); 
		}
	}
}
return RETURN_SUCCESS;
}

void stop_condition(const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
{
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
char i2c_bus_configure(struct i2c_bus_details *i2c_bus_details)
{
	if(!i2c_bus_details)
		return INVARG;
	struct i2c_bus_details *temp;

	gpio_output_type(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, OPEN_DRAIN);
	gpio_pull_set   (i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PULL_UP);
	gpio_output_state(i2c_bus_details->sda_port, i2c_bus_details->sda_pin, PIN_HIGH);
	gpio_mode_set   (i2c_bus_details->sda_port, i2c_bus_details->sda_pin, OUTPUT_MODE);

	gpio_output_type(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, OPEN_DRAIN);
	gpio_pull_set   (i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PULL_UP);
	gpio_output_state(i2c_bus_details->scl_port, i2c_bus_details->scl_pin, PIN_HIGH);
	gpio_mode_set   (i2c_bus_details->scl_port, i2c_bus_details->scl_pin, OUTPUT_MODE);

	temp = realloc(bus_table, (bus_count + 1) * sizeof(struct i2c_bus_details));

	if(temp == NULL)
		return -RETURN_FAILURE;      
	bus_table = temp;
	bus_table[bus_count] = *i2c_bus_details;
	bus_count++;

	return (bus_count - 1);
}

char i2c_transfer(struct i2c_target_details *i2c_target_details, struct i2c_transfer_details *i2c_write_details, struct i2c_transfer_details *i2c_read_details)
{
	if((!i2c_target_details) || (i2c_target_details->target_address > 127) || (i2c_target_details->bus_id >= bus_count))
		return INVARG;
	if(i2c_write_details)
	{
		if(!i2c_write_details->data_buffer || !i2c_write_details->byte_count)
			return INVARG;
	}

	if(i2c_read_details)
	{
		if(!i2c_read_details->data_buffer || !i2c_read_details->byte_count)
			return INVARG;
	}

	unsigned char delay, address_byte;
	char ret;

	delay = (1000000 / (i2c_target_details->mode*1000)) / 2;

	start_condition((bus_table + i2c_target_details->bus_id), delay);
	address_byte = (i2c_target_details->target_address << 1) |((i2c_read_details && !i2c_write_details) ? I2C_READ : I2C_WRITE);

	ret = i2c_send(&address_byte, 1 , (bus_table + i2c_target_details->bus_id), delay);

	if(ret || (!i2c_write_details && !i2c_read_details))
	{
		stop_condition((bus_table + i2c_target_details->bus_id), delay);
		return ret;
	}

	if(i2c_write_details)
		ret = i2c_send(i2c_write_details->data_buffer, 
				i2c_write_details->byte_count, 
				(bus_table + i2c_target_details->bus_id), delay);
	if(i2c_read_details)
	{
		if(i2c_write_details)
		{
			start_condition((bus_table + i2c_target_details->bus_id), delay);
			address_byte = (i2c_target_details->target_address << 1) | I2C_READ;
			ret = i2c_send(&address_byte, 1 , (bus_table + i2c_target_details->bus_id), delay);
			if(ret)
			{

				stop_condition((bus_table + i2c_target_details->bus_id), delay);
				return ret;
			}
		}
		ret = i2c_receive(i2c_read_details->data_buffer, 
				i2c_read_details->byte_count, 
				(bus_table + i2c_target_details->bus_id), delay);
	}

	stop_condition((bus_table + i2c_target_details->bus_id), delay);
	return ret;
}

