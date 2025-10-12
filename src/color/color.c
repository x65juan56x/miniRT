#include "../../include/color.h"
#include "../../include/vec3.h"
#include "../../include/math_utils.h"

inline uint32_t	rgba_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (((uint32_t)r << 24) | ((uint32_t)g << 16)
		| ((uint32_t)b << 8) | (uint32_t)a);
}

inline uint8_t	clamp_u8i(int v)
{
	if (v < 0)
		return (0);
	if (v > 255)
		return (255);
	return ((uint8_t)v);
}

uint32_t	vec3_to_rgba(t_vec3 rgb)
{
	int	 r;
	int	 g;
	int	 b;

	r = (int)(clampf(rgb.x, 0.0f, 1.0f) * 255.0f + 0.5f);
	g = (int)(clampf(rgb.y, 0.0f, 1.0f) * 255.0f + 0.5f);
	b = (int)(clampf(rgb.z, 0.0f, 1.0f) * 255.0f + 0.5f);
	return (rgba_u32(clamp_u8i(r), clamp_u8i(g), clamp_u8i(b), 255));
}
/*
Propósito: Convertir un color en formato vector (float [0,1]) a un entero RGBA (uint32_t).
Lógica:
- Limita cada componente (x, y, z) al rango [0,1] usando clampf.
- Multiplica por 255 y suma 0.5 para redondear antes de convertir a int.
- clamp_u8i asegura que el valor final está en [0,255] (uint8_t).
- rgba_u32 empaqueta los componentes en un uint32_t (orden RGBA).
Variables:
- rgb: color como vector (float x, y, z).
- r, g, b: componentes enteros del color.
*/
/*
El +0.5f en la fórmula:
Se usa para redondear correctamente el valor flotante al entero más cercano.
Sin el +0.5f, el cast a (int) simplemente trunca (corta decimales hacia abajo), lo que puede sesgar los colores hacia valores menores.
Por ejemplo:
Si el resultado es 127.8, con +0.5f → 128.3 → (int) → 128 (correcto).
Si el resultado es 127.2, con +0.5f → 127.7 → (int) → 127.
Es una técnica estándar para convertir de float a int con redondeo en vez de truncamiento.
*/
