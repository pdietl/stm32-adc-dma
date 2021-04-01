# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_STM32_TIMER_TRIG hwtrig_stm32_timer.c)
