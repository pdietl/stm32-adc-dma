Analog-to-Digital Converter (ADC)
#################################

Overview
********

This repository implements stm32 drivers for adc, using hardware trigger
and dma. When you call `adc_read_async` the adc will start to read the
samples to a buffer using dma in circular buffer mode.

Each adc conversion will start after hardware
trigger event has occurred. In the current implemenation, we also provide
a hardware trigger using a timer (I used a patch to the pwm driver because
of my lazyness). Look at the `dts/bindings` on how to configure the driver.

The dma interrupt will occur on half transmission, which is after half of the
size of the buffer given in `adc_read_async` was filled. After that the
application will send the samples over usb.


.. note::

   The adc driver needs to get a callback after half transmit complete interrupt.
   The current zephyr dma driver not enabling this interrupt. Thus, you need also
   https://github.com/zephyrproject-rtos/zephyr/pull/32713.

