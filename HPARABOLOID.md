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
---
---

# Paraboloide hiperbólico (hp)

Este documento explica en detalle cómo se representa e interseca el paraboloide hiperbólico en este proyecto, cómo se describe en el archivo .rt y cómo se usan sus parámetros durante el trazado de rayos, recorte, normales y sombreado.

## Sintaxis en el archivo .rt

Tipo de objeto: `hp`

Formato general (7 argumentos):

hp center axis rx ry height color

Ejemplo:

hp 0,0,0 0,1,0 2.0 3.0 3.5 180,120,255

Significado de cada argumento y su efecto geométrico:
- center (vec3): centro del sólido en espacio mundo. Es el origen local del sistema (u, v, axis). Mueve la superficie completa.
- axis (vec3 normalizado): eje del paraboloide en mundo. Define la dirección del eje z local. Rota la superficie para que su “columna” se alinee con este vector.
- rx (float > 0): semi-eje en el eje local x para el recorte elíptico de la sección transversal. Visualmente, estira/encoge el lóbulo negativo en X (apertura en X).
- ry (float > 0): semi-eje en el eje local y para el recorte elíptico. Visualmente, estira/encoge el lóbulo positivo en Y (apertura en Y).
- height (float > 0): altura total del recorte a lo largo del eje del paraboloide (axis). Se usa como recorte en |z| <= height (clamp vertical completo). Visualmente, “corta” el paraboloide arriba y abajo con mayor holgura que el recorte a la mitad.
- color (RGB 0..255): albedo del objeto. Se normaliza a [0,1] por canal y se usa en el sombreado Lambert.

Notas prácticas:
- El parser valida normalización de axis y valores positivos para rx, ry, height.
- Los colores en 0..255 se convierten a floats en [0,1].
- La escena de ejemplo `scenes/ejemplo_hp_suite.rt` reúne varios hp para comprobar rx/ry/height y un caso con axis inclinado.

## Representación interna (`t_hparab`)

Estructura definida en `include/scene.h`:
- center: centro en mundo.
- axis: eje local z (normalizado).
- u, v: ejes ortonormales locales x, y construidos perpendiculares a axis.
- rx, ry: radios de recorte en los ejes u y v.
- height: altura total; half_height = height (recorte vertical simétrico a altura completa).
- inv_rx2, inv_ry2: inversos precalculados de rx^2 y ry^2 para rendimiento.
- inv_height: 1.0/height (se usa en la forma implícita). Si height → 0 no es válido.
- color: albedo en [0,1]^3.

Estos valores se fijan en el parser y quedan cacheados para evitar recomputaciones por rayo.

## Ecuación implícita y sistema local

Trabajamos en el marco local del objeto donde:
- u, v son los ejes locales x e y (perpendiculares a axis).
- axis es el eje local z.

La forma implícita usada es (equivalente a z = y^2/ry^2 − x^2/rx^2):

F(x, y, z) = y^2/ry^2 − x^2/rx^2 − z/height = 0

Observaciones:
- El término −z/height simplemente escala el eje z para que el recorte vertical sea consistente con height.
- rx, ry controlan la “apertura” en los ejes u y v: secciones elípticas con radios rx y ry.

Para pasar del mundo al local, proyectamos:

rel = r.orig − center
ox = dot(rel, u), oy = dot(rel, v), oz = dot(rel, axis)
dx = dot(r.dir, u), dy = dot(r.dir, v), dz = dot(r.dir, axis)

## Intersección rayo-superficie

Sustituyendo (x, y, z) = (ox + t dx, oy + t dy, oz + t dz) en F = 0 se obtiene una cuadrática en t:

A t^2 + B t + C = 0, con
A = dy^2/ry^2 − dx^2/rx^2
B = 2 oy dy/ry^2 − 2 ox dx/rx^2 − dz/height
C = oy^2/ry^2 − ox^2/rx^2 − oz/height

Detalles de implementación (`src/geom/hparaboloid_bonus.c`):
- Se proyecta el rayo al sistema local y se montan A, B, C usando inv_ry2, inv_rx2, inv_height cacheados.
- Si |A| < 1e−8, la cuadrática degenera en lineal: B t + C = 0.
- Si disc = B^2 − 4 A C < 0, no hay intersección.
- Si hay soluciones, se prueban ambas con un filtro geométrico.

## Recorte geométrico (bounds)

El paraboloide es infinito; lo acotamos con dos condiciones:
1) Recorte elíptico lateral: (x^2/rx^2 + y^2/ry^2) <= 1 + 1e−4
2) Recorte vertical: |z| <= half_height + 2e−4 (con half_height = height)

El pequeño margen 1e−4 estabiliza errores numéricos en los bordes.
Además, se exige t > EPSILON para evitar auto-intersecciones.

## Normal

En la intersección aceptada, la normal se calcula por el gradiente de F en local:

∇F = (∂F/∂x, ∂F/∂y, ∂F/∂z) = (−2x/rx^2, 2y/ry^2, −1/height)

Se normaliza y se transforma a mundo con la base (u, v, axis). La orientación final se ajusta contra la dirección del rayo para que apunte hacia el hemisferio correcto.

Nota: La construcción de la normal se hace en `record_hparaboloid` dentro de `src/core/intersect.c`.

## Integración con el render y sombreado

- La ruta de intersección llama a `hit_hparaboloid`, que devuelve el t más cercano válido.
- Si hay hit, `scene_hit` prepara el `t_hit` con punto, normal orientada y albedo.
- El sombreador Lambert (`shade_lambert`) usa el albedo del hp, la luz puntual y la luz ambiente; también respeta sombras con un rayo secundario hacia la luz.

## Parser y validaciones

El parser de `hp` (en `src/parse/parse_objects.c`):
- Lee center (vec3), axis (vec3) normalizado, rx, ry, height (floats > 0) y color (RGB 0..255).
- Construye el marco ortonormal (u, v, axis). Si axis ~ up_world, elige un up alternativo para una base estable.
- Cachea half_height, inv_rx2, inv_ry2, inv_height.
- Inserta el objeto en la lista de la escena.

Errores comunes detectados:
- Axis no normalizado.
- Radios/height no positivos.
- Color fuera de 0..255.

## Resumen visual de parámetros

- center: traslada la superficie completa.
- axis: rota la superficie; define hacia dónde “sube/baja” el paraboloide.
- rx: abre/cierra la curvatura en la dirección u (ancho de la ‘silla’ en X local).
- ry: abre/cierra la curvatura en la dirección v (ancho de la ‘silla’ en Y local).
- height: limita el objeto arriba/abajo con dos planos perpendiculares a axis.
- color: color base usado en el cálculo de difuso y combinado con la luz ambiente.

## Notas numéricas

- EPSILON = 1e−4 para ignorar intersecciones a t ≈ 0.
- Tolerancias: elíptica 1e−4; vertical 2e−4 para robustez adicional en el clamp superior/inferior.
- Se usan inversos precalculados para minimizar multiplicaciones/divisiones por rayo.

## Ejemplo anotado

hp 0,0,0 0,1,0 2.0 3.0 3.5 180,120,255

- center = (0,0,0)
- axis = (0,1,0) → eje del hp apunta al +Y mundo
- rx = 2.0, ry = 3.0 → elipse de recorte lateral ancha en v
- height = 3.5 → clamp vertical en |z_local| ≤ 3.5 (half_height = height)
- color = (180,120,255)/255 → albedo ~ (0.706, 0.471, 1.0)

Con esto, el hp luce como una “silla de montar” centrada en el origen, extendida más a lo largo de v (ry=3) que de u (rx=2), recortada por una elipse lateral y verticalmente a |z_local| ≤ height.
- Elige el menor $t > epsilon$ (para evitar self-intersection).


