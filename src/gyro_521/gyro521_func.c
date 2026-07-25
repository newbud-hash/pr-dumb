#include "gy521.h"
#include <stdio.h>

#define TEMP_OUT         0x41
#define GYRO_OUT         0x43
#define ACCEL_OUT        0x3B
#define WAKE_REG         0x6B
#define STANDARD_MODE    100
#define MSB_INDEX        8
#define DATA_2B          2
#define ALL_AXIS_BYTE    6

static char read_data(const struct gy_target_details *gy_target,
                      unsigned char                   gy_reg,
                      unsigned char                   byte_count,
                      void                           *receive_buffer)
{
    unsigned char raw_buffer[byte_count];
    short         *buf;
    char          ret;  

    struct i2c_transfer_details write;
    struct i2c_transfer_details read;

    I2C_TARGET_DETAILS(target, gy_target->address, STANDARD_MODE, gy_target->bus);

    buf = (short *)receive_buffer;
    write.byte_count  = 1;
    write.data_buffer = &gy_reg;

    read.byte_count   = byte_count;
    read.data_buffer  = raw_buffer;

    ret = i2c_transfer(&target, &write, &read);
    if(ret) {
        return ret;
    }

    buf[0] = (short)(raw_buffer[0] << MSB_INDEX | raw_buffer[1]);
    if(byte_count == ALL_AXIS_BYTE) {
        buf[1] = (short)(raw_buffer[2] << MSB_INDEX | raw_buffer[3]);
        buf[2] = (short)(raw_buffer[4] << MSB_INDEX | raw_buffer[5]);
    }

    return ret;
}

char gy521_initialize(const struct gy_target_details *gy_target_details)
{
    unsigned char               wake_up[2];
    struct i2c_transfer_details wake;

    if(!gy_target_details || !gy_target_details->bus) {
        return -INVARG;
    }

    I2C_TARGET_DETAILS(target, gy_target_details->address, STANDARD_MODE,
                       gy_target_details->bus);

    if(i2c_transfer(&target, NULL, NULL)) {
        return -RETURN_FAILURE;
    }

    printf("target 0x%x found on bus\n", gy_target_details->address);

    wake_up[0] = WAKE_REG;
    wake_up[1] = 0x00;

    wake.byte_count  = 2;
    wake.data_buffer = wake_up;

    if(i2c_transfer(&target, &wake, NULL)) {
        return -RETURN_FAILURE;
    }
    return RETURN_SUCCESS;
}

char gyroscope_read(const struct gy_target_details *gy_target_details,
                    enum axis                       axis,
                    void                           *receive_buffer)
{
    if(!gy_target_details || !gy_target_details->bus || !receive_buffer) {
        return -INVARG;
    }

    if(axis < X || axis > XYZ) {
        return -INVARG;
    }

    if(axis == XYZ) {
        return read_data(gy_target_details, GYRO_OUT, ALL_AXIS_BYTE, receive_buffer);
    }
    else {
	    return read_data(gy_target_details, GYRO_OUT + (axis * 2), DATA_2B, receive_buffer);
    }
}

char accelerometer_read(const struct gy_target_details *gy_target_details,
                        enum axis                       axis,
                        void                           *receive_buffer)
{
    if(!gy_target_details || !gy_target_details->bus || !receive_buffer) {
        return -INVARG;
    }
    if(axis < X || axis > XYZ) {
	    return -INVARG;
    }

    if(axis == XYZ) {
        return read_data(gy_target_details, ACCEL_OUT, ALL_AXIS_BYTE, receive_buffer);
    }
    else {
        return read_data(gy_target_details, ACCEL_OUT + (axis * 2), DATA_2B, receive_buffer);
    }
}

char temperature_read(const struct gy_target_details *gy_target_details,
                      float                          *temperature_buffer)
{
    char ret;
    short temp_receive = 0;

    if(!gy_target_details || !gy_target_details->bus || !temperature_buffer) {
        return -INVARG;
    }

    ret = read_data(gy_target_details, TEMP_OUT, DATA_2B, &temp_receive);
    if(ret) {
	    return ret;
    }
    
    *temperature_buffer = ((float)temp_receive / 340.0f) + 36.53f;

    return ret;
}
