#include "i2c_driver.h"

static const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

int i2c_driver_init(void)
{
		if (!i2c_dev) {
		printk("I2C: Device driver not found.\n");
		return -1;
		}
		
		return i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_STANDARD));
}

int i2c_driver_write_bytes(uint16_t addr)
{
	uint8_t wr_addr[2];
	struct i2c_msg msgs[2];

	wr_addr[0] = (addr >> 8) & 0xFF;
	wr_addr[1] = addr & 0xFF;

	msgs[0].buf = wr_addr;
	msgs[0].len = 2U;
	msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	/* Data to be written, and STOP after this. */
	// msgs[1].buf = data;
	// msgs[1].len = num_bytes;
	// msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 1, SCD41_I2C_ADDR);
}

int i2c_driver_read_bytes(uint16_t addr, uint8_t *data, uint32_t num_bytes)
{
	uint8_t wr_addr[2];
	struct i2c_msg msgs[2];

	wr_addr[0] = (addr >> 8) & 0xFF;
	wr_addr[1] = addr & 0xFF;

	msgs[0].buf = wr_addr;
	msgs[0].len = 2U;
	msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, 0x62);
}

bool i2c_driver_is_device_available(uint8_t address)
{
	struct i2c_msg msgs[1];
	uint8_t dst = 1;

	/* Send the address to read from */
	msgs[0].buf = &dst;
	msgs[0].len = 1U;
	msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;
	
	int error = i2c_transfer(i2c_dev, &msgs[0], 1, address);
	return error == 0;
}

uint8_t i2c_driver_get_first_device_address(void)
{
	for (uint8_t i = 4; i <= 0x77; i++) {
	struct i2c_msg msgs[1];
	uint8_t dst = 1;

	/* Send the address to read from */
	msgs[0].buf = &dst;
	msgs[0].len = 1U;
	msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;
	
	int error = i2c_transfer(i2c_dev, &msgs[0], 1, i);
	if (error == 0) {
		printk("0x%2x FOUND\n", i);
		return i;
	}
	else {
		printk("error %d \n", error);
	}
}
	return 0;
}