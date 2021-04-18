# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_EXTERNAL_ADC adc)
add_subdirectory_ifdef(CONFIG_EXTERNAL_DAC dac)
add_subdirectory_ifdef(CONFIG_HWTRIG hwtrig)
