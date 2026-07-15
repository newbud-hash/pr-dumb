#include "stm32.h"

#define GPIO_BASE         0x40020000
#define GPIO_PORT_OFFSET  0x0400

volatile gpio_reg *port_base_addr(gpio_port port_name)
{
	return ((volatile gpio_reg *)(GPIO_BASE + (GPIO_PORT_OFFSET*port_name)); 
}

void gpio_output_state(gpio_port port_name, unsigned char pin, gpio_pin_state state)
{
        volatile gpio_reg *port = port_base_addr(port_name);
        port->output_data = (state ? port->output_data | (1 << pin) : port->output_data & (~(1 << pin)));
}

void gpio_output_type(gpio_port port_name, unsigned char pin, gpio_outpin_type output_type)
{
	volatile gpio_reg *port = port_base_addr(port_name);
	port->output_type &= ~(1 << pin);
	port->output_type |= (output_type << pin);
}

void gpio_output_toggle(gpio_port port_name, unsigned char pin)
{
	volatile gpio_reg *port = port_base_addr(port_name);
	port->output_data ^= (1 << pin);
}

void gpio_mode_set(gpio_port port_name, unsigned char pin, gpio_mode mode_type)
{
        volatile gpio_reg *port = port_base_addr(port_name);
	port->mode &= ~(3 << pin*2);
	port->mode |= (mode_type << pin*2);
}

void gpio_pull_set(gpio_port port_name, unsigned char pin, gpio_updown pull_type)
{
	volatile gpio_reg *port = port_base_addr(port_name);
	port->pullup_down &= ~(3 << pin*2);
	port->pullup_down |= (pull_type << pin*2);
}

gpio_pin_state gpio_input_read(gpio_port port_name, unsigned char pin)
{
	volatile gpio_reg *port = port_base_addr(port_name);
	return (((port->input_data) >> pin) & 1);
}

