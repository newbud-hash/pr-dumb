#include "gy521.h"
#include <stdio.h>

#define TARGET_ADDR   0x68
#define TEMP_OUT      0x41
#define GYRO_OUT      0x43
#define ACCEL_OUT     0x3B
#define WAKE_REG      0x6B
#define STANDARD_MODE 100

static char read_data(const struct gy_target_details *gy_target,
                      unsigned char gy_reg,
                      unsigned char bytes,
                      unsigned char *raw_buffer)
{
    if(!gy_target || !gy_target->bus_details || !raw_buffer)
        return INVARG;

    struct i2c_target_details target1;
    struct i2c_transfer_details write;
    struct i2c_transfer_details read;

    target1.target_address = TARGET_ADDR | gy_target->ad0_state;
    target1.mode           = STANDARD_MODE;
    target1.bus_details    = gy_target->bus_details;

    write.byte_count  = 1;
    write.data_buffer = &gy_reg;

    read.byte_count  = bytes;
    read.data_buffer = raw_buffer;

    if(i2c_transfer(&target1, &write, &read))
        return -RETURN_FAILURE;

    return RETURN_SUCCESS;
}

char gy521_initialize(const struct gy_target_details *gy_target_details)
{
    if(!gy_target_details || !gy_target_details->bus_details)
        return INVARG;

    struct i2c_target_details   target1;
    struct i2c_transfer_details wake;
    unsigned char wake_up[2] = {WAKE_REG, 0x00};

    target1.target_address = TARGET_ADDR | gy_target_details->ad0_state;
    target1.mode           = STANDARD_MODE;
    target1.bus_details    = gy_target_details->bus_details;

    if(i2c_transfer(&target1, 0, 0))
        return -RETURN_FAILURE;
    printf("target 0x%x found on bus\n", target1.target_address);

    wake.byte_count  = 2;
    wake.data_buffer = wake_up;

    if(i2c_transfer(&target1, &wake, 0))
        return -RETURN_FAILURE;

    return RETURN_SUCCESS;
}

char gyroscope_read(const struct gy_target_details *gy_target_details,
                    enum axis axis,
                    short *receive_buffer)
{
    if(!gy_target_details || !gy_target_details->bus_details || !receive_buffer)
        return INVARG;

    if(axis != X && axis != Y && axis != Z && axis != XYZ)
        return INVARG;

    unsigned char raw_gyro[6] = {0};

    if(axis == XYZ)
    {
        if(read_data(gy_target_details, GYRO_OUT, 6, raw_gyro))
            return -RETURN_FAILURE;

        receive_buffer[0] = (raw_gyro[0] << 8) | raw_gyro[1];
        receive_buffer[1] = (raw_gyro[2] << 8) | raw_gyro[3];
        receive_buffer[2] = (raw_gyro[4] << 8) | raw_gyro[5];
    }
    else
    {
        if(read_data(gy_target_details, GYRO_OUT + (axis * 2), 2, raw_gyro))
            return -RETURN_FAILURE;

        *receive_buffer = (raw_gyro[0] << 8) | raw_gyro[1];
    }

    return RETURN_SUCCESS;
}

char accelerometer_read(const struct gy_target_details *gy_target_details,
                        enum axis axis,
                        short *receive_buffer)
{
    if(!gy_target_details || !gy_target_details->bus_details || !receive_buffer)
        return INVARG;

    if(axis != X && axis != Y && axis != Z && axis != XYZ)
        return INVARG;

    unsigned char raw_acc[6] = {0};

    if(axis == XYZ)
    {
        if(read_data(gy_target_details, ACCEL_OUT, 6, raw_acc))
            return -RETURN_FAILURE;

        receive_buffer[0] = (raw_acc[0] << 8) | raw_acc[1];
        receive_buffer[1] = (raw_acc[2] << 8) | raw_acc[3];
        receive_buffer[2] = (raw_acc[4] << 8) | raw_acc[5];
    }
    else
    {
        if(read_data(gy_target_details, ACCEL_OUT + (axis * 2), 2, raw_acc))
            return -RETURN_FAILURE;

        *receive_buffer = (raw_acc[0] << 8) | raw_acc[1];
    }

    return RETURN_SUCCESS;
}

char temperature_read(const struct gy_target_details *gy_target_details,
                      float *temperature_buffer)
{
    if(!gy_target_details || !gy_target_details->bus_details || !temperature_buffer)
        return INVARG;

    unsigned char raw_temp[2] = {0};
    short         temp_receive;

    if(read_data(gy_target_details, TEMP_OUT, 2, raw_temp))
        return -RETURN_FAILURE;

    temp_receive        = (raw_temp[0] << 8) | raw_temp[1];
    *temperature_buffer = ((float)temp_receive / 340.0f) + 36.53f;

    return RETURN_SUCCESS;
}
