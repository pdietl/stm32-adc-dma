# SPDX-License-Identifier: Apache-2.0

if(CONFIG_STM32_EXTERNAL_DRIVERS)
  zephyr_include_directories(include)

  zephyr_library()
  zephyr_library_sources_ifdef(CONFIG_STM32_ADC_DMA src/adc_stm32_dma.c)
  zephyr_library_sources_ifdef(CONFIG_STM32_PWM_TRIG src/pwm_stm32_trig.c)

endif()
