/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief DAC public API header file.
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_DAC_EXT_H_
#define ZEPHYR_INCLUDE_DRIVERS_DAC_EXT_H_

#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DAC driver APIs
 * @defgroup dac_ext_interface DAC driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @struct dac_ext_channel_cfg
 * @brief Structure for specifying the configuration of a DAC channel.
 *
 * @param channel_id Channel identifier of the DAC that should be configured.
 * @param resolution Desired resolution of the DAC (depends on device
 *                   capabilities).
 */
struct dac_ext_channel_cfg {
	uint8_t channel_id;
	uint8_t resolution;
	bool enable_hw_trigger;
};

/**
 * @typedef dac_callback_t
 * @brief Define the application callback function signature for
 * dac_ext_callback_set() function.
 *
 * @param dev       DAC device structure.
 * @param user_data Pointer to data specified by user.
 */
typedef void (*dac_callback_t)(const struct device *dev,
				uint8_t channel, void *user_data);

/**
 * @cond INTERNAL_HIDDEN
 *
 * For internal use only, skip these in public documentation.
 */

/*
 * Type definition of DAC API function for configuring a channel.
 * See dac_ext_channel_setup() for argument descriptions.
 */
typedef int (*dac_ext_api_channel_setup)(const struct device *dev,
				     const struct dac_ext_channel_cfg *channel_cfg);

/*
 * Type definition of DAC API function for setting a write request.
 * See dac_ext_write_value() for argument descriptions.
 */
typedef int (*dac_ext_api_write_value)(const struct device *dev,
				    uint8_t channel, uint32_t value);

/*
 * Type definition of DAC API function for setting a callback
 * for continuos api.
 * See dac_ext_callback_set() for argument descriptions.
 */
typedef int (*dac_ext_api_callback_set)(const struct device *dev,
				    dac_callback_t callback, void *user_data);

/*
 * Type definition of DAC API function for start dac in
 * continuos mode.
 * See dac_ext_start_continuous() for argument descriptions.
 */
typedef int (*dac_ext_api_start_continuous)(const struct device *dev,
					uint8_t channel);

/*
 * Type definition of DAC API function for fill dac buffer in
 * continuos mode.
 * See dac_ext_fill_buffer() for argument descriptions.
 */
typedef int (*dac_ext_api_fill_buffer)(const struct device *dev, uint8_t channel,
					uint8_t *data, size_t size);

/*
 * DAC driver API
 *
 * This is the mandatory API any DAC driver needs to expose.
 */
__subsystem struct dac_ext_driver_api {
	dac_ext_api_channel_setup channel_setup;
	dac_ext_api_write_value   write_value;
#ifdef CONFIG_DAC_CONTINUOUS_API
	dac_ext_api_callback_set callback_set;
	dac_ext_api_start_continuous start_continuous;
	dac_ext_api_fill_buffer fill_buffer;
#endif
};

/**
 * @endcond
 */

/**
 * @brief Configure a DAC channel.
 *
 * It is required to call this function and configure each channel before it is
 * selected for a write request.
 *
 * @param dev          Pointer to the device structure for the driver instance.
 * @param channel_cfg  Channel configuration.
 *
 * @retval 0         On success.
 * @retval -EINVAL   If a parameter with an invalid value has been provided.
 * @retval -ENOTSUP  If the requested resolution is not supported.
 */
__syscall int dac_ext_channel_setup(const struct device *dev,
				const struct dac_ext_channel_cfg *channel_cfg);

static inline int z_impl_dac_ext_channel_setup(const struct device *dev,
					   const struct dac_ext_channel_cfg *channel_cfg)
{
	const struct dac_ext_driver_api *api =
				(const struct dac_ext_driver_api *)dev->api;

	return api->channel_setup(dev, channel_cfg);
}

/**
 * @brief Write a single value to a DAC channel
 *
 * @param dev         Pointer to the device structure for the driver instance.
 * @param channel     Number of the channel to be used.
 * @param value       Data to be written to DAC output registers.
 *
 * @retval 0        On success.
 * @retval -EINVAL  If a parameter with an invalid value has been provided.
 */
__syscall int dac_ext_write_value(const struct device *dev, uint8_t channel,
			      uint32_t value);

static inline int z_impl_dac_ext_write_value(const struct device *dev,
						uint8_t channel, uint32_t value)
{
	const struct dac_ext_driver_api *api =
				(const struct dac_ext_driver_api *)dev->api;

	return api->write_value(dev, channel, value);
}

/**
 * @brief Set event handler function.
 * 
 * This function will be called when the dac device is free to receive
 * more samples to send.
 *
 * @param dev       DAC device structure.
 * @param callback  Event handler.
 * @param user_data Data to pass to event handler function.
 *
 * @retval -ENOTSUP If not supported.
 * @retval 0	    If successful, negative errno code otherwise.
 */
__syscall int dac_ext_callback_set(const struct device *dev,
				    dac_callback_t callback,
				    void *user_data);

static inline int z_impl_dac_ext_callback_set(const struct device *dev,
				    dac_callback_t callback,
				    void *user_data)
{
#ifdef CONFIG_DAC_CONTINUOUS_API
	const struct dac_ext_driver_api *api =
				(const struct dac_ext_driver_api *)dev->api;

	return api->callback_set(dev, callback, user_data);
#else
	return -ENOTSUP;
#endif
}

/**
 * @brief Start DAC in continuous mode.
 * 
 * This will start the dac driver to send continuously the samples
 * it has in its buffer.
 *
 * @param dev       DAC device structure.
 * @param channel   Number of the channel to be used.
 *
 * @retval -ENOTSUP If not supported.
 * @retval 0	    If successful, negative errno code otherwise.
 */
__syscall int dac_ext_start_continuous(const struct device *dev,
					uint8_t channel);

static inline int z_impl_dac_ext_start_continuous(const struct device *dev,
					uint8_t channel)
{
#ifdef CONFIG_DAC_CONTINUOUS_API
	const struct dac_ext_driver_api *api =
				(const struct dac_ext_driver_api *)dev->api;

	return api->start_continuous(dev, channel);
#else
	return -ENOTSUP;
#endif
}

/**
 * @brief Start DAC in continuous mode.
 *
 * @param dev       DAC device structure.
 * @param channel   Number of the channel to be used.
 * @param data      samples to send.
 * @param size      size of data buffer.
 *
 * @retval -ENOTSUP If not supported.
 * @retval number of bytes sent,	    If successful.
 */
__syscall int dac_ext_fill_buffer(const struct device *dev, uint8_t channel,
					uint8_t *data, size_t size);

static inline int z_impl_dac_ext_fill_buffer(const struct device *dev, uint8_t channel,
					uint8_t *data, size_t size)
{
#ifdef CONFIG_DAC_CONTINUOUS_API
	const struct dac_ext_driver_api *api =
				(const struct dac_ext_driver_api *)dev->api;

	return api->fill_buffer(dev, channel, data, size);
#else
	return -ENOTSUP;
#endif
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#include <syscalls/dac_ext.h>

#endif  /* ZEPHYR_INCLUDE_DRIVERS_DAC_EXT_H_ */
