#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>
#include "bsp/i2c_driver.h"
#include "scd41/scd41.h"

#define MOVING_AVERAGE_WINDOW_SIZE (10u)
#define HUMIDITY_THRESHOLD (60u)

static int32_t movingAvg(int *ptrArrNumbers, uint32_t *ptrSum, size_t pos, uint16_t nextNum)
{
	*ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
	ptrArrNumbers[pos] = nextNum;
	return *ptrSum / MOVING_AVERAGE_WINDOW_SIZE;
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

	uint32_t co2AvgArray[MOVING_AVERAGE_WINDOW_SIZE] = {0};
	uint32_t temperatureAvgArray[MOVING_AVERAGE_WINDOW_SIZE] = {0};
	uint32_t humidityAvgArray[MOVING_AVERAGE_WINDOW_SIZE] = {0};

	size_t pos = 0;
	int32_t co2Avg = 0;
	int32_t temperatureAvg = 0;
	int32_t humidityAvg = 0;

	int co2Sum = 0;
	int temperatureSum = 0;
	int humiditySum = 0;

	bool moving_averge_filled = false;

	while (1)
	{
		uint16_t co2Sample = 0;
		uint16_t temperatureSample = 0;
		uint16_t humiditySample = 0;
		k_sleep(K_SECONDS(5U));
		int16_t rc = scd41_get_measures(&co2Sample, &temperatureSample, &humiditySample);
		printk("co2 sample %d\n", co2Sample);
		printk("temperature sample %d\n", temperatureSample);
		printk("humidity sample %d\n", humiditySample);

		if (rc == 0)
		{
			co2Avg = movingAvg(co2AvgArray, &co2Sum, pos, co2Sample);
			temperatureAvg = movingAvg(temperatureAvgArray, &temperatureSum, pos, temperatureSample);
			humidityAvg = movingAvg(humidityAvgArray, &humiditySum, pos, humiditySample);
		}

		pos++;
		if (pos >= MOVING_AVERAGE_WINDOW_SIZE)
		{
			moving_averge_filled = true;
			pos = 0;
		}
		if (moving_averge_filled)
		{
			// printk("average temperature value: %d\n", temperatureAvg);

			if(humidityAvg >= (HUMIDITY_THRESHOLD - 2)  && humidityAvg <= (HUMIDITY_THRESHOLD + 2))
			{
				printk("Humidity threshold reached. Checking temperature trend\n");

				if (temperatureSample < temperatureAvg)
				{
					printk("Temperature dropping\n");
					printk("Report to HVAC controller to heat up\n");
				}
				else
				{
					printk("Temperature rising\n");
					printk("co2 concentration: %d\n", co2Avg);
					printk("humidity value: %d\n", humidityAvg);

					printk("if trend is rising report that mold is growing\n");
				}
			}
			else
			{
				printk("Low humidity value %d, no mold grow possible\n", humidityAvg);
			}
		}
	}
}
