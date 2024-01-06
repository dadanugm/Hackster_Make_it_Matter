/*

 */

#include "adc_app.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lib/support/CodeUtils.h>

/*
CUSTOM OVERLAY POINTER
*/
#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

uint16_t buf;
/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)};

struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

void AdcApp::AdcTest()
{
	int err;
	uint32_t count = 0;

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
		}
	}

	printk("ADC reading[%u]:\n", count++);

		for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
			int32_t val_mv;
			printk("- %s, channel %d: ", adc_channels[i].dev->name, adc_channels[i].channel_id);

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			err = adc_read(adc_channels[i].dev, &sequence);
			if (err < 0) {
				printk("Could not read (%d)\n", err);
			}

			val_mv = (int32_t)buf;
			printk("%"PRId32, val_mv);

			err = adc_raw_to_millivolts_dt(&adc_channels[i], &val_mv);
			/* conversion to mV may not be supported, skip if not */
			if (err < 0) {
				printk(" (value in mV not available)\n");
			} else {
				printk(" = %"PRId32" mV\n", val_mv);
			}
		}

		k_sleep(K_MSEC(1000));
}

// Initialize sensor
void AdcApp::SoilSensorInit(uint8_t channel)
{
	int err;

	printk("Initialize ADC Soil sensor\n");
	if (!adc_is_ready_dt(&adc_channels[channel])) {
		printk("ADC controller device %s not ready\n", adc_channels[channel].dev->name);
	}

	err = adc_channel_setup_dt(&adc_channels[channel]);
	if (err < 0) {
		printk("Could not setup channel #%d (%d)\n", channel, err);
	}

	(void)adc_sequence_init_dt(&adc_channels[channel], &sequence);

	k_sleep(K_MSEC(100));
}


// Read sensor data
uint8_t AdcApp::ReadSoilSensor(uint8_t channel)
{
	uint8_t sensor_val;
	int32_t val_mv;
	float temp_val_mv;
	int err;

	printk("Read Soil Sensor Value\n");
	// Get ADC Data channel 0
	err = adc_read(adc_channels[channel].dev, &sequence);
	if (err < 0) {
		printk("Could not read (%d)\n", err);
	}

	val_mv = (int32_t)buf;
	err = adc_raw_to_millivolts_dt(&adc_channels[channel], &val_mv);
			/* conversion to mV may not be supported, skip if not */
	if (err < 0) {
		printk(" (value in mV not available)\n");
	} else {
		printk(" ADC Val = %"PRId32" mV\n", val_mv);
	}

	// convert 
	temp_val_mv = (float(val_mv)/1807) ;
	sensor_val = uint8_t (temp_val_mv * 100);
	printk("float temp_val (%f)\n", temp_val_mv);
	printk("sensor_val (%d)\n", sensor_val);

	k_sleep(K_MSEC(100));

	return sensor_val;
}

/* EOF*/
