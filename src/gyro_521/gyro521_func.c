#include "gy521.h"
#include <stdio.h>

#define TEMP_OUT         0x41
#define GYRO_OUT         0x43
#define ACCEL_OUT        0x3B
#define WAKE_REG         0x6B
#define STANDARD_MODE    100
#define MSB_INDEX        8
#define SINGLE_AXIS_BYTE 2
#define XYZ_AXIS_BYTE    6

static char read_data(const struct gy_target_details *gy_target,
                      unsigned char                   gy_reg,
                      unsigned char                   byte_count,
                      void                           *receive_buffer)
{
    unsigned char raw_buffer[XYZ_AXIS_BYTE] = {0};
    short        *buf = (short *)receive_buffer;

    struct i2c_transfer_details write;
    struct i2c_transfer_details read;

    I2C_TARGET_DETAILS(target, gy_target->address, STANDARD_MODE, gy_target->bus);

    write.byte_count  = 1;
    write.data_buffer = &gy_reg;

    read.byte_count   = byte_count;
    read.data_buffer  = raw_buffer;

    char ret = i2c_transfer(&target, &write, &read);
    if(ret)
        return ret;

    buf[0] = (short)(raw_buffer[0] << MSB_INDEX | raw_buffer[1]);
    if(byte_count == XYZ_AXIS_BYTE)
    {
        buf[1] = (short)(raw_buffer[2] << MSB_INDEX | raw_buffer[3]);
        buf[2] = (short)(raw_buffer[4] << MSB_INDEX | raw_buffer[5]);
    }

    return ret;
}

char gy521_initialize(const struct gy_target_details *gy_target_details)
{
    unsigned char               wake_up[2] = {WAKE_REG, 0x00};
    struct i2c_transfer_details wake;

    if(!gy_target_details || !gy_target_details->bus)
        return -INVARG;

    I2C_TARGET_DETAILS(target, gy_target_details->address, STANDARD_MODE,
                       gy_target_details->bus);

    if(i2c_transfer(&target, NULL, NULL))
        return -RETURN_FAILURE;
    printf("target 0x%x found on bus\n", gy_target_details->address);

    wake.byte_count  = 2;
    wake.data_buffer = wake_up;

    if(i2c_transfer(&target, &wake, NULL))
        return -RETURN_FAILURE;

    return RETURN_SUCCESS;
}

char gyroscope_read(const struct gy_target_details *gy_target_details,
                    enum axis                       axis,
                    void                           *receive_buffer)
{
    if(!gy_target_details || !gy_target_details->bus || !receive_buffer)
        return -INVARG;

    if(axis > XYZ)
        return -INVARG;

    if(axis == XYZ)
        return read_data(gy_target_details, GYRO_OUT, XYZ_AXIS_BYTE, receive_buffer);
    else
        return read_data(gy_target_details, GYRO_OUT + (axis * 2), SINGLE_AXIS_BYTE, receive_buffer);
}

char accelerometer_read(const struct gy_target_details *gy_target_details,
                        enum axis                       axis,
                        void                           *receive_buffer)
{
    if(!gy_target_details || !gy_target_details->bus || !receive_buffer)
        return -INVARG;

    if(axis > XYZ)
        return -INVARG;

    if(axis == XYZ)
        return read_data(gy_target_details, ACCEL_OUT, XYZ_AXIS_BYTE, receive_buffer);
    else
        return read_data(gy_target_details, ACCEL_OUT + (axis * 2), SINGLE_AXIS_BYTE, receive_buffer);
}

char temperature_read(const struct gy_target_details *gy_target_details,
                      float                          *temperature_buffer)
{
    unsigned char               raw_temp[2] = {0};
    unsigned char               temp_reg    = TEMP_OUT;
    struct i2c_transfer_details temp_write;
    struct i2c_transfer_details temp_read;
    char                        ret;

    if(!gy_target_details || !gy_target_details->bus || !temperature_buffer)
        return -INVARG;

    I2C_TARGET_DETAILS(target, gy_target_details->address, STANDARD_MODE,
                       gy_target_details->bus);

    temp_write.byte_count  = 1;
    temp_write.data_buffer = &temp_reg;

    temp_read.byte_count   = 2;
    temp_read.data_buffer  = raw_temp;

    ret = i2c_transfer(&target, &temp_write, &temp_read);
    if(ret)
        return ret;

    *temperature_buffer = ((short)(raw_temp[0] << MSB_INDEX | raw_temp[1]) / 340.0f) + 36.53f;

    return ret;
}
