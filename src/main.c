#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>
#include "bsp/i2c_driver.h"
#include "scd41/scd41.h"

void main(void)
{
	i2c_driver_init();
	printk("Starting i2c scanner...\n");
	k_sleep(K_MSEC(1U));

	uint16_t s0 = 0;
	uint16_t s1 = 0; 
	uint16_t s2 = 0; 

	int rc = scd41_get_serial_number(&s0, &s1, &s2);
	printk("rc %d\n", rc);
	printk("s0: %d s1: %d s2: %d\n", s0, s1, s2);

	scd41_start_periodic_measurement();

	while(1)
	{
		uint16_t co2 = 0;
		uint16_t temperature = 0;
		uint16_t humidity = 0;
		scd41_get_measures(&co2, &temperature, &humidity);
		printk("co2 %d\n", co2);
		printk("temperature %d\n",   temperature);
		printk("humidity %d\n", humidity);
		k_sleep(K_SECONDS(5U));
	}
}
