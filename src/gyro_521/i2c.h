#ifndef I2C_H
#define I2C_H

#include "stm32.h"

#define I2C_WRITE	0
#define I2C_READ	1

#define RETURN_SUCCESS 0
#define RETURN_FAILURE 1
#define INVARG         2

struct i2c_bus_details {
	gpio_port     sda_port;
	unsigned char sda_pin;
	gpio_port     scl_port;
	unsigned char scl_pin;
};

struct i2c_transfer_details {
	unsigned short	byte_count;
	unsigned char	*data_buffer;
};

struct i2c_target_details {
	unsigned char target_address;
	unsigned char mode; /* standard mode - 100khz */
	struct i2c_bus_details *bus_details;
};

/* api for user */
struct i2c_bus_details *i2c_bus_configure(struct i2c_bus_details *i2c_bus_details);
char i2c_transfer(struct i2c_target_details   *i2c_target_details, 
		  struct i2c_transfer_details *i2c_write_details,
	          struct i2c_transfer_details *i2c_read_details);


/*
 * helper apis for i2c stack:
 *char i2c_send(unsigned char *data_buffer, unsigned short byte_count, const struct i2c_bus_details *i2c_bus_details, unsigned char delay)
char i2c_receive(unsigned char *data_buffer, unsigned short byte_count, const struct i2c_bus_details *i2c_bus_details, unsigned char delay)

 * 
 * void stop_condition(struct i2c_bus_details *i2c_bus_details, 
                       unsigned char delay);
 * void stop_condition(struct i2c_bus_details *i2c_bus_details, 
                       unsigned char delay);
 */

#endif
