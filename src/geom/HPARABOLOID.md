# Paraboloide Hiperbólica

## 1. Ecuación implícita y parametrización
La ecuación implícita es:

$$ z = y²/b² - x²/a² $$

Reordenada:

$$ y²/b² - x²/a² - z = 0 $$

## 2. Sustitución del rayo
Un rayo se define como:

$$ r(t) = o + t*d $$

donde $o = (o_x, o_y, o_z)$ es el origen y $d = (d_x, d_y, d_z)$ la dirección.

Sustituimos $x = o_x + t d_x, y = o_y + t d_y, z = o_z + t d_z$ en la ecuación del paraboloide:

$$ (o_y + t d_y)^2 / b^2 - (o_x + t d_x)^2 / a^2 - (o_z + t d_z) = 0 $$

## 3. Expansión y agrupación
Expande los cuadrados y agrupa por potencias de t:
- $(o_y + t d_y)^2 = o_y^2 + 2 o_y d_y t + d_y^2 t^2$
- $(o_x + t d_x)^2 = o_x^2 + 2 o_x d_x t + d_x^2 t^2$

Sustituyendo:

$$ (o_y^2 + 2 o_y d_y t + d_y^2 t^2)/b^2 - (o_x^2 + 2 o_x d_x t + d_x^2 t^2)/a^2 - o_z - d_z t = 0 $$

Agrupa por $t^2, t,$ y términos independientes:

$$ A t^2 + B t + C = 0 $$

donde:
- $A = (d_y^2)/b^2 - (d_x^2)/a^2$
- $B = (2 o_y d_y)/b^2 - (2 o_x d_x)/a^2 - d_z$
- $C = (o_y^2)/b^2 - (o_x^2)/a^2 - o_z$

## 4. Resolución de la cuadrática
Resuelve $A t^2 + B t + C = 0$ para $t$:

$$ t = (-B ± sqrt(B^2 - 4AC)) / (2A) $$

- Si el discriminante $B^2 - 4AC < 0$, no hay intersección.
- Elige el menor $t > epsilon$ (para evitar self-intersection).

## 5. Punto y normal
- Punto de intersección: $p = o + t*d$
- Normal en $p$: gradiente de la función implícita:
$$
    grad F(x, y, z) = (
      -2x/a^2,
       2y/b^2,
       -1
    )
$$
Normaliza este vector.

