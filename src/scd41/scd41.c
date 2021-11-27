#include "scd41.h"
#include "i2c_driver.h"

static uint16_t scd41_bytes_to_uint16_t(const uint8_t *bytes)
{
    return (uint16_t)bytes[0] << 8 | (uint16_t)bytes[1];
}

int16_t scd41_get_serial_number(uint16_t *serial_0, uint16_t *serial_1, uint16_t *serial_2)
{
    int16_t error;
    uint8_t buffer[9] = {0};

    error = i2c_driver_write_bytes(0x3682);
    if (error != 0)
    {
        return error;
    }

    k_sleep(K_MSEC(1U));

    error = i2c_driver_read_bytes(0x3682, &buffer[0], 9);
    if (error != 0)
    {
        return error;
    }
    *serial_0 = scd41_bytes_to_uint16_t(&buffer[0]);
    *serial_1 = scd41_bytes_to_uint16_t(&buffer[3]);
    *serial_2 = scd41_bytes_to_uint16_t(&buffer[5]);
    return 0;
}

int16_t scd41_start_periodic_measurement(void)
{
    return i2c_driver_write_bytes(0x21B1);
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
        *co2 = scd41_bytes_to_uint16_t(&data[0]);
        uint16_t temperature_tmp = scd41_bytes_to_uint16_t(&data[3]);
        uint16_t humidity_tmp = scd41_bytes_to_uint16_t(&data[5]);
        *temperature = -45U + ((175U * temperature_tmp) / 65536U);
        *humidity = ((100U * humidity_tmp)/65536U);
    }
    return 0;
}