#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>
#include "bsp/i2c_driver.h"
#include "scd41/scd41.h"

static int movingAvg(int *ptrArrNumbers, int *ptrSum, int pos, int len, int nextNum)
{
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  ptrArrNumbers[pos] = nextNum;
  return *ptrSum / len;
}

static volatile int32_t avgCo2 = 600; // initial value 
void main(void)
{
	i2c_driver_init();
	printk("Starting i2c scanner...\n");
	k_sleep(K_MSEC(100U));

	scd41_wake_up();
	k_sleep(K_MSEC(100U));
	scd41_stop_periodic_measurement();
	k_sleep(K_MSEC(500U));
	scd41_reinit();
	k_sleep(K_MSEC(100U));

	uint16_t s0 = 0;
	uint16_t s1 = 0; 
	uint16_t s2 = 0; 

	int rc = scd41_get_serial_number(&s0, &s1, &s2);
	printk("rc %d\n", rc);
	printk("s0: %d s1: %d s2: %d\n", s0, s1, s2);

	scd41_start_periodic_measurement();

	int arrNumbers[5] = {0};
	int pos = 0;
	int co2Avg = 0;
	int sum = 0;
	int len = sizeof(arrNumbers) / sizeof(int);
	bool moving_averge_filled = false;

	while(1)
	{
		uint16_t co2 = 0;
		uint32_t temperature = 0;
		uint16_t humidity = 0;
		k_sleep(K_SECONDS(5U));
		scd41_get_measures(&co2, &temperature, &humidity);
		printk("co2 %d\n", co2);
		printk("temperature %d\n",   temperature);
		printk("humidity %d\n", humidity);		

		if(co2 > 0)
		{
			co2Avg = movingAvg(arrNumbers, &sum, pos, len, co2);
		}

		pos++;
		if (pos >= len){
			moving_averge_filled = true;
			pos = 0;
		}
		if(moving_averge_filled)
			printk("moving average co2 %d\n", co2Avg);
		}
}
