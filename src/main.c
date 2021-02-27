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

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define ADC_NUM_CHANNELS	DT_PROP_LEN(DT_PATH(zephyr_user), io_channels)

#if ADC_NUM_CHANNELS > 1
#error "Currently only 1 channel supported in this sample"
#endif

#define ADC_NODE		DT_PHANDLE(DT_PATH(zephyr_user), io_channels)

/* Common settings supported by most ADCs */
#define ADC_RESOLUTION		12
#define ADC_GAIN		ADC_GAIN_1
#define ADC_REFERENCE		ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME_DEFAULT

/* Get the numbers of up to two channels */
static uint8_t channel_ids[ADC_NUM_CHANNELS] = {
	DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(zephyr_user), 0),
#if ADC_NUM_CHANNELS == 2
	DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(zephyr_user), 1)
#endif
};

static int16_t sample_buffer[10000];

struct adc_channel_cfg channel_cfg = {
	.gain = ADC_GAIN,
	.reference = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	/* channel ID will be overwritten below */
	.channel_id = 0,
	.differential = 0
};

struct adc_sequence sequence = {
	/* individual channels will be added below */
	.channels    = 0,
	.buffer      = sample_buffer,
	/* buffer size in bytes, not number of samples */
	.buffer_size = sizeof(sample_buffer),
	.resolution  = ADC_RESOLUTION,
};

static void send_all_usb(const struct device *dev, uint8_t *buffer, size_t size)
{
	size_t count = size;
	while (count > 0)
	{
		count -= uart_fifo_fill(dev, buffer, count);
	}
}

struct k_poll_signal async_sig;

void main(void)
{
	int err;
	const struct device *dev_adc = DEVICE_DT_GET(ADC_NODE);
	const struct device *dev_pwm = device_get_binding("PWM_2");
	const struct device *dev_usb = device_get_binding("CDC_ACM_0");

	if (!dev_usb) {
		printk("CDC ACM device not found");
		return;
	}

	int ret = usb_enable(NULL);
	if (ret != 0) {
		printk("Failed to enable USB\n");
		return;
	}

	pwm_pin_set_usec(dev_pwm, 2, 1000, 500, 0);

	if (!device_is_ready(dev_adc)) {
		printk("ADC device not found\n");
		return;
	}

	k_poll_signal_init(&async_sig);

	struct k_poll_event async_evt =
		K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL,
					 K_POLL_MODE_NOTIFY_ONLY,
					 &async_sig);

	/*
	 * Configure channels individually prior to sampling
	 */
	for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
		channel_cfg.channel_id = channel_ids[i];
#ifdef CONFIG_ADC_NRFX_SAADC
		channel_cfg.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0
					     + channel_ids[i];
#endif

		adc_channel_setup(dev_adc, &channel_cfg);

		sequence.channels |= BIT(channel_ids[i]);
	}

	bool first_half = true;
	int half_buffer_size = sizeof(sample_buffer) / 2;

	err = adc_read_async(dev_adc, &sequence, &async_sig);
	if (err != 0) {
		printk("ADC reading failed with error %d.\n", err);
		return;
	}

	while (true)
	{
		k_poll(&async_evt, 1, K_FOREVER);
		async_evt.signal->signaled = 0;
		async_evt.state = K_POLL_STATE_NOT_READY;

		if (first_half) {
			send_all_usb(dev_usb,
					(uint8_t*)sample_buffer, half_buffer_size);
			first_half = false;
		} else {
			send_all_usb(dev_usb,
					((uint8_t*)sample_buffer) + half_buffer_size, half_buffer_size);
			first_half = true;
		}
	}
}
