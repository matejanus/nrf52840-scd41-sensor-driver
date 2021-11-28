#pragma once

#include <stdint.h>

#define CRC8_POLYNOMIAL 0x31
#define CRC8_INIT 0xFF
#define CRC_ERROR 1

#define SENSIRION_COMMAND_SIZE 2
#define SENSIRION_WORD_SIZE 2
#define SENSIRION_NUM_WORDS(x) (sizeof(x) / SENSIRION_WORD_SIZE)
#define SENSIRION_MAX_BUFFER_WORDS 32
#define BYTE_NUM_ERROR 4
#define CRC8_LEN 1

int16_t scd41_reinit(void);
int16_t scd41_get_serial_number(uint16_t *serial_0, uint16_t *serial_1, uint16_t *serial_2);
int16_t scd41_start_periodic_measurement(void);
int16_t scd41_stop_periodic_measurement(void);
int16_t scd41_get_measures(uint16_t *co2, uint16_t *temperature, uint16_t *humidity);
void scd41_wake_up(void);