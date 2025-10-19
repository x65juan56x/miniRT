#include <math.h>
#include "../../include/math_utils.h"

float	deg2rad(float d)
{
	return (d * (float)M_PI / 180.0f);
}

float	clampf(float value, float min, float max)
{
	if (value < min)
		return (min);
	if (value > max)
		return (max);
	return (value);
}
/*
Propósito: Limitar (clamp) un valor flotante al rango [min, max].
Lógica:
- Si el valor es menor que min, devuelve min.
- Si el valor es mayor que max, devuelve max.
- Si está en el rango, devuelve el valor original.
*/

/*PROBAR SI CON ESTA RAIZ CUADRADA FUNCIONA MAS RÁPIDO*/
// float q_rsqrt(float number)
// {
// 	long		i;
// 	float		x2;
// 	float		y;
// 	const float	threehalfs = 1.5f;

// 	x2 = number * 0.5f;
// 	y = number;
// 	i = *(long *)&y;
// 	i = 0x5f3759df - (i >> 1);
// 	y = *(float *)&i;
// 	y = y * (threehalfs - (x2 *y *y));
// 	y = y * (threehalfs - (x2 *y *y));

// 	return (y);
// }