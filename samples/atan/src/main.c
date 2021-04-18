/*
 * Copyright (c) 2021 Shlomi Vaknin
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <math/fast_atan.h>

void main(void)
{
	float ret = atan_f32(0.45f);
	printf("atan(0.45) = %f\n", ret);

	ret = atan2_f32(0.24f, 0.213f);
	printf("atan2(0.24, 0.213) = %f\n", ret);
}
