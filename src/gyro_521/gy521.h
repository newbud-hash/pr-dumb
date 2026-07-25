#ifndef GYRO_H
#define GYRO_H

#include "i2c.h"

#define AD0_LOW  0
#define AD0_HIGH 1

#define GY_TARGET_DETAILS(name, bus, address)  \
    struct gy_target_details name = {          \
        .bus = (void *)(bus),                  \
        .address     = (address)               \
    }

enum axis {
    X = 0,
    Y,
    Z,
    XYZ
};

struct gy_target_details {
    void          *bus;  
    unsigned char address;      
};

char gy521_initialize  (const struct gy_target_details *gy_target_details);
char gyroscope_read    (const struct gy_target_details *gy_target_details, enum axis axis, void *receive_buffer);
char accelerometer_read(const struct gy_target_details *gy_target_details, enum axis axis, void *receive_buffer);
char temperature_read  (const struct gy_target_details *gy_target_details, float *temperature_buff);

#endif
