#pragma once

#include <stdint.h>

int16_t scd41_get_serial_number(uint16_t *serial_0, uint16_t *serial_1, uint16_t *serial_2);
int16_t scd41_start_periodic_measurement(void);
int16_t scd41_get_measures(uint16_t *co2, uint16_t *temperature, uint16_t *humidity);