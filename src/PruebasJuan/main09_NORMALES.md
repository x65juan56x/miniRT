## Visualización de Normales – main09

En `main09_normals.c` añadimos un modo de depuración para visualizar las normales de superficie como colores. Es una herramienta clave para verificar la orientación de las superficies, la coherencia de las intersecciones y el cálculo correcto de las normales por objeto.

### Objetivo

- Alternar entre el sombreado convencional (ambient + Lambert) y un modo “normales” que pinta cada punto en función de su normal $N$.
- Confirmar que las normales estén normalizadas, orientadas hacia el rayo y sin discontinuidades.

### ¿Qué es una normal y cómo la usamos?

- La normal $N$ es un vector unitario perpendicular a la superficie en un punto.
- Se usa para calcular el ángulo con la luz (Lambert), reflexiones, refracciones, etc.
- En este modo debug la convertimos a color para inspección visual.

### Cálculo de normales por forma

- Esfera: $N = \text{normalize}(P - C)$, donde $P$ es el punto de impacto y $C$ el centro.
- Plano: $N$ es constante y viene del parser (ya normalizado). Para caras traseras, solemos reorientarla si $\langle N, r.dir \rangle > 0$.

En `intersect_scene(...)`:
- Guardamos `hit.n` y la orientamos para que mire hacia la cámara (si `dot(N, r.dir) > 0`, multiplicamos por `-1`).

### Mapear normales a color

Las normales tienen componentes en $[-1, 1]$. Para verlas como color en $[0,1]$ hacemos un remapeo lineal por componente:

$$
N_{color} = 0.5 \cdot (N + 1)
$$

En código: `v3_mul(v3_add(N, v3(1,1,1)), 0.5f)`.

Luego convertimos a RGBA con `vec3_to_rgba`.

### Integración en el render

1. Intersección: `intersect_scene` devuelve `hit.ok`, `hit.p`, `hit.n`, `hit.albedo` (igual que en main07/08).
2. Toggle de modo: un flag `show_normals` vive en `t_app` y se alterna con la tecla `N`.
3. Render por píxel (`render_scene`):
   - Si `show_normals && hit.ok`: usar el color mapeado de la normal.
   - En otro caso: usar `shade_lambert(...)` como siempre.

### Detalles y pitfalls

- Normalización: Asegura que `N` esté normalizada; de lo contrario, el mapeo a color será errático.
- Orientación: Si no reorientas `N` cuando la cara está de espaldas al rayo, verás saltos de color al cruzar contornos.
- Espacio de color: El mapeo es puramente visual; no lo confundas con espacio tangente o normales de texturas (normal maps), que son otro tema.
- Objetos nuevos: Cuando añadas cilindros/triángulos, implementa la normal correcta por forma para que el modo debug sea útil.

### Siguientes pasos

- Visualización combinada: colorear por normales pero oscurecer con `max(0, N·L̂)` para sugerir forma y orientación a la vez.
- Mostrar longitud de `N` (diagnóstico): colorear por `|N|` para detectar errores de normalización.
- Añadir un HUD simple con el estado del toggle.
---
1. Verificar orientación de superficies
- Si una esfera o plano se ve “oscuro” o con iluminación incorrecta, puede ser porque la normal está invertida.
- Con el modo normales, verás claramente si las normales apuntan hacia fuera (correcto) o hacia dentro (error).
2. Detectar discontinuidades y artefactos
- Si hay saltos bruscos de color en el modo normales, puede indicar:
    - Discontinuidad en el cálculo de normales.
    - Problemas en la interpolación (en objetos más complejos).
    - Errores en la lógica de inversión de normales para caras traseras.
3. Depurar intersecciones
- Si un objeto no aparece o se ve “cortado”, el modo normales te ayuda a ver si realmente hay intersección y qué normal se está calculando en cada punto.
- Si ves negro donde debería haber color, puede ser que no haya hit o que la normal esté mal calculada.
4. Validar nuevos objetos
- Cuando implementes cilindros, triángulos, etc., el modo normales te permite comprobar rápidamente si el cálculo de la normal es correcto en toda la superficie.
5. Comprobar normalización
- Si el color de las normales no varía suavemente, puede ser que no estés normalizando el vector, lo que afectará la iluminación.
6. Preparar materiales avanzados
- Antes de implementar reflejos, refracciones o normal mapping, necesitas asegurarte de que las normales básicas están bien.
7. Visualizar la geometría de la escena
- El modo normales te da una “visión de rayos X” de la orientación de todas las superficies, útil para entender la composición espacial.

**En resumen:**  
El toggle de normales es una herramienta de diagnóstico rápido para detectar y corregir errores en el cálculo y orientación de las normales, fundamentales para que la iluminación y los materiales funcionen correctamente.
¡Muchos bugs de shading se resuelven en segundos con este modo!