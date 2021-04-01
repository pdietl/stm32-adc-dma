# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

list(APPEND ZEPHYR_EXTRA_MODULES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../stm32-external-drivers
  )

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(ADC2USB)

target_sources(app PRIVATE src/main.c)
