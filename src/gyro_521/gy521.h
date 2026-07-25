#ifndef GYRO_H
#define GYRO_H

#include "i2c.h"

#define AD0_LOW		0
#define AD0_HIGH	1

enum axis {
	X = 0,
	Y,
	Z,
	XYZ
};

struct gy_target_details {
	struct i2c_bus_details *bus_details;
	unsigned char ad0_state;
};

char gy521_initialize(const struct gy_target_details *gy_target_details);

/* initialize() 
 * 
 * check the ad0_state and OR with the gy_521 sensors address 0x68 
 * check the target existence
 * wake up the sensor from sleep mode 
 */

char gyroscope_read(const struct gy_target_details *gy_target_details, enum axis axis, short *receive_buffer);

/* gyroscope_read()
 *
 * call read data function based on how many access going to be read
 * check the ad0_state and OR with the gy_521 sensors address 0x68 
 * 
 * based on the axis read the data from the internal registers of the gyroscope 
 *	axis X data - 0x43 to 0x44
 *	axis Y data - 0x45 to 0x46
 *	axis Z data - 0x47 to 0x48
 * 
 * update the modified data in the receive buffer
 * return SUCCESS / FAILURE
 */

char accelerometer_read(const struct gy_target_details *gy_target_details, enum axis axis, short *receive_buffer);

/* accelerometer_read()
 *  
 * call read data function based on how many access going to be read
 * check the ad0_state and OR with the gy_521 sensors address 0x68 
 * 
 * based on the axis read the data from the internal registers of the gyroscope 
 *	axis X data - 0x3B to 0x3C
 *	axis Y data - 0x3D to 0x3E
 *	axis Z data - 0x3F to 0x40
 * 
 * update the modified data in the receive buffer
 * return SUCCESS / FAILURE
 */

char temperature_read(const struct gy_target_details *gy_target_details, float *temperature_buff);

/* temperature_read()
 *
 * call read data function based on how many access going to be read
 * check the ad0_state and OR with the gy_521 sensors address 0x68 
 * Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
 * update the modified data in the receive buffer
 * return SUCCESS / FAILURE
 */
#endif
