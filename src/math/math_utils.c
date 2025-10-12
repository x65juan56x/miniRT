#include <math.h>
#include "../../include/math_utils.h"

float deg2rad(float d)
{
    return (d * (float)M_PI / 180.0f);
}

float clampf(float value, float min, float max)
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
