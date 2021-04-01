/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/adc.h>
#include <drivers/pwm.h>
#include <usb/usb_device.h>
#include <drivers/uart.h>
#include <logging/log.h>
#include <drivers/dac.h>
#include <drivers/hwtrig.h>
#include <drivers/dac_ext.h>

LOG_MODULE_REGISTER(adc_acm, LOG_LEVEL_INF);

static const struct dac_channel_cfg dac_ch_cfg1 = {
	.channel_id  = 1,
	.resolution  = 12
};

static uint16_t values1[4096];

uint16_t curr_values1_pos = 0;

void my_dac_callback(const struct device *dev_dac, uint8_t channel, void *arg)
{
	if (channel == 1) {
		int ret = dac_ext_fill_buffer(dev_dac, channel,
				(uint8_t *)&values1[curr_values1_pos], 4096);
		curr_values1_pos = (curr_values1_pos + 2048) % 4096;
	}
}

void main(void)
{
	int ret;
	const struct device *dev_trig = device_get_binding("TRIG_2");
	const struct device *dev_dac = device_get_binding("DAC_1");

	if (!dev_dac) {
		printk("DAC device not found\n");
		return;
	}

	if (!dev_trig) {
		printk("TRIG device not found\n");
		return;
	}

	ret = dac_channel_setup(dev_dac, &dac_ch_cfg1);

	if (ret != 0) {
		printk("Setting up of DAC channel failed with code %d\n", ret);
		return;
	}

	dac_ext_callback_set(dev_dac, my_dac_callback, NULL);

	ret = dac_ext_start_continuous(dev_dac, 1);

	for (uint16_t i = 0; i < ARRAY_SIZE(values1); ++i) {
		values1[i] = i;
	}

	dac_ext_fill_buffer(dev_dac, 1, (uint8_t *)values1, sizeof(values1));
	curr_values1_pos = 2048;

	k_sleep(K_SECONDS(10));

	// uint32_t freq = 40000; // 40 [KHz]
	uint32_t freq = 1000; // 1 [KHz]
	hwtrig_enable(dev_trig, &freq);
}
