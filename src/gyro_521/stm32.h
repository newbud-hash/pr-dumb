#ifndef STM32_H
#define STM32_H

typedef struct{
        unsigned int mode;
        unsigned int output_type;
        unsigned int output_speed;
        unsigned int pullup_down;
        unsigned int input_data;
        unsigned int output_data;
}gpio_reg;

typedef enum{
        GPIO_PORT_A,
        GPIO_PORT_B,
        GPIO_PORT_C,
        GPIO_PORT_D,
        GPIO_PORT_H = 7
}gpio_port;

typedef enum{
        INPUT_MODE,
        OUTPUT_MODE,
        ALTERNATE_FUNCTION,
        ANALOG_MODE
}gpio_mode;

typedef enum{
        NOPULL,
        PULL_UP,
        PULL_DOWN
}gpio_updown;

typedef enum{
        PIN_LOW,
        PIN_HIGH
}gpio_pin_state;

typedef enum{
	PUSH_PULL,
	OPEN_DRAIN
}gpio_outpin_type;

volatile gpio_reg *port_base_addr(gpio_port port_name);
void gpio_output_state(gpio_port port_name, unsigned char pin, gpio_pin_state state);
void gpio_output_toggle(gpio_port port_name, unsigned char pin);
void gpio_mode_set(gpio_port port_name, unsigned char pin, gpio_mode mode_type);
void gpio_pull_set(gpio_port port_name, unsigned char pin, gpio_updown pull_type);
gpio_pin_state gpio_input_read(gpio_port port_name, unsigned char pin);
void gpio_output_type(gpio_port port_name, unsigned char pin, gpio_outpin_type output_type);

#endif
