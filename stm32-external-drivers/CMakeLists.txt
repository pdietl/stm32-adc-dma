# SPDX-License-Identifier: Apache-2.0

if(CONFIG_STM32_EXTERNAL_DRIVERS)
  zephyr_include_directories(include)

  add_subdirectory(drivers)
  add_subdirectory(apps)

endif()
