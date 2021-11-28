#include "scd41.h"
#include "i2c_driver.h"

static uint16_t scd41_bytes_to_uint16_t(const uint8_t *bytes)
{
    return (uint16_t)bytes[0] << 8 | (uint16_t)bytes[1];
}

static uint8_t scd41_generate_crc(const uint8_t *data, uint16_t count)
{
    uint8_t crc = CRC8_INIT;

    /* calculates 8-Bit checksum with given polynomial */
    for (uint16_t current_byte = 0; current_byte < count; ++current_byte)
    {
        crc ^= (data[current_byte]);
        for (uint8_t crc_bit = 8; crc_bit > 0; --crc_bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

static int8_t scd41_check_crc(const uint8_t *data, uint16_t count, uint8_t checksum)
{
    if (scd41_generate_crc(data, count) != checksum)
        return CRC_ERROR;
    return 0;
}

static int16_t scd41_read_data_inplace(uint8_t *buffer,
                                       uint16_t expected_data_length)
{
    int16_t error = 0;

    if (expected_data_length % SENSIRION_WORD_SIZE != 0)
    {
        return BYTE_NUM_ERROR;
    }

    const uint16_t size = (expected_data_length / SENSIRION_WORD_SIZE) *
                          (SENSIRION_WORD_SIZE + CRC8_LEN);

    for (uint16_t i = 0, j = 0; i < size; i += SENSIRION_WORD_SIZE + CRC8_LEN)
    {

        error = scd41_check_crc(&buffer[i], SENSIRION_WORD_SIZE,
                                buffer[i + SENSIRION_WORD_SIZE]);
        if (error)
        {
            return error;
        }
        buffer[j++] = buffer[i];
        buffer[j++] = buffer[i + 1];
    }

    return 0;
}

int16_t scd41_reinit(void)
{
    return i2c_driver_write_bytes(0x3646);
}

int16_t scd41_get_serial_number(uint16_t *serial_0, uint16_t *serial_1, uint16_t *serial_2)
{
    int16_t error;
    uint8_t buffer[9] = {0};

    error = i2c_driver_read_bytes(0x3682, &buffer[0], 9);
    if (error != 0)
    {
        return error;
    }

    error = scd41_read_data_inplace(&buffer[0], 6);
    if (error)
    {
        return error;
    }

    *serial_0 = scd41_bytes_to_uint16_t(&buffer[0]);
    *serial_1 = scd41_bytes_to_uint16_t(&buffer[2]);
    *serial_2 = scd41_bytes_to_uint16_t(&buffer[4]);
    return 0;
}

int16_t scd41_start_periodic_measurement(void)
{
    return i2c_driver_write_bytes(0x21B1);
}

int16_t scd41_stop_periodic_measurement(void)
{
    return i2c_driver_write_bytes(0x3F86);
}

int16_t scd41_get_measures(uint16_t *co2, uint16_t *temperature, uint16_t *humidity)
{
    uint8_t data[9] = {0};
    int rc = i2c_driver_read_bytes(0xec05, data, 9);

    if (rc != 0)
    {
        return rc;
    }
    else
    {
        rc = scd41_read_data_inplace(&data[0], 6);
        if (rc)
        {
            return rc;
        }
        *co2 = scd41_bytes_to_uint16_t(&data[0]);
        uint16_t temperature_tmp = scd41_bytes_to_uint16_t(&data[2]);
        uint16_t humidity_tmp = scd41_bytes_to_uint16_t(&data[4]);
        *temperature = -45U + ((175U * temperature_tmp) / 65536U);
        *humidity = ((100U * humidity_tmp) / 65536U);
    }
    return 0;
}

void scd41_wake_up(void)
{
    i2c_driver_write_bytes(0x36F6);
}