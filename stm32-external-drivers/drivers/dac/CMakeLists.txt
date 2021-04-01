# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_STM32_DAC_DMA dac_stm32_dma.c)
