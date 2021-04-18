/*
 * Copyright (c) 2021 Shlomi Vaknin
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_FAST_ATAN_H_
#define ZEPHYR_INCLUDE_FAST_ATAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FAST_ATAN_TABLE_SIZE    (512)

/*
 * Computes the trigonometric inverse tangent function using a combination of
 * table lookup and linear interpolation.
 * Currently, there is only function for floating-point data type.
 * The input to the floating-point version can be any real number.
 * The implementation takes advantage of the following identities:
 *  -# atan(-x) = -atan(x)
 *  -# atan(x) = pi/2 - atan(1/x)
 * Thus, the algorithm needs to compute values in [0,1].
 * The implementation is based on table lookup using 512 values together with
 * linear interpolation.
 * The steps used are:
 *  -# Calculation of the nearest integer table index
 *  -# Compute the fractional portion (fract) of the table index.
 *  -# The final result equals <code>(1.0f-fract)*a + fract*b;</code>
 * where
 * <pre>
 *    b = Table[index];
 *    c = Table[index+1];
 * </pre>
 */
float atan_f32(float x);

float atan2_f32(float y, float x);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_DRIVERS_HWTRIG_H_ */
