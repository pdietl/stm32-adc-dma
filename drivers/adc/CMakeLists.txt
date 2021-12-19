# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_STM32_ADC_DMA adc_stm32_dma.c)
