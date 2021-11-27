#pragma once
#include <drivers/i2c.h>
#include <device.h>

#define FRAM_I2C_ADDR	0x62

#define SCD41_I2C_ADDR	0x62

int i2c_driver_init(void);
int i2c_driver_write_bytes(uint16_t addr);
int i2c_driver_read_bytes(uint16_t addr, uint8_t *data, uint32_t num_bytes);
uint8_t i2c_driver_get_first_device_address(void);
bool i2c_driver_is_device_available(uint8_t address);
