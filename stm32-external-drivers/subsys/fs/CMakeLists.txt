# SPDX-License-Identifier: Apache-2.0

if(CONFIG_FILE_SYSTEM_SHELL)

  zephyr_library()
  zephyr_library_sources(shell.c)

endif()