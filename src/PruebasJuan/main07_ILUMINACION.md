## Iluminación Ambient + Lambert

- Luz ambiental: una contribución constante en toda la escena. Sirve para “levantar” la oscuridad general. En el código viene de `scene->ambient`.
- Difuso Lambertiano: la intensidad recibida en un punto es proporcional al coseno del ángulo entre la normal de la superficie `N` y la dirección hacia la luz `L̂`.
  - Si la luz llega perpendicular a la superficie ($N$ y $L$ alineados, ángulo $0º$), toda la energía incide en el área mínima: el coseno es 1, y la intensidad es máxima.
  - Si la luz llega en ángulo, la misma energía se reparte en un área mayor (la proyección de la luz sobre la superficie es menor): el coseno es menor que 1, y la intensidad baja.
  - Si la luz llega tangencialmente ($90º$), el coseno es 0, y no hay iluminación directa.
  - Matemáticamente: $I_\text{dif} = k_d \cdot \max(0, \langle N, L̂ \rangle)$
  - “k_d” aquí lo representamos con el albedo del material (color intrínseco) y la potencia/color de la luz.
  - El producto escalar $N \cdot L̂$ da el coseno del ángulo $θ$ entre esos dos vectores: $N \cdot L̂ = |N||L̂|cosθ$. Como ambos son unitarios, $|N|=|L̂|=1$, así que $N \cdot L̂= cosθ$
  - Si la luz llega “por detrás” ($θ>90º$), $cosθ$ es negativo, pero físicamente no tiene sentido que la luz ilumine la parte trasera, así que usamos $max(0,N \cdot L̂)$ para que nunca sea negativo.

### Datos mínimos por píxel

Para cada rayo primario (uno por píxel) necesitamos:

1. Punto de impacto P: `ray_at(r, t)` donde `t` es la distancia al primer objeto intersectado.
2. Normal en el punto N: 
	- Esfera: `normalize(P - centro)`
	- Plano: `normal` constante del plano
	- Aseguramos que la normal apunte “hacia la cámara” para evitar caras traseras brillando: si `dot(N, r.dir) > 0` invertimos `N`.
3. Albedo (color del material): viene del objeto intersectado.
4. Dirección hacia la luz L̂: `normalize(light.pos - P)`

### Fórmula de color que aplicamos

Usamos colores en espacio lineal [0, 1]. La salida por píxel es:

$$
C = C_\text{amb} + (A \odot C_\text{luz} \cdot \text{bright}) \cdot \max(0, \langle N, L̂ \rangle)
$$

Donde:

- $C_\text{amb} = C_\text{ambient} \cdot \text{ratio}$ de la escena.
- $A$ es el albedo (color del material) de la superficie en P.
- $C_\text{luz}$ es el color de la luz y `bright` su intensidad.
- $\odot$ es multiplicación componente a componente (R, G, B).

Entonces:

- $C_\text{amb}$ es la luz ambiental: simula la iluminación indirecta, que llega a todas partes aunque no haya luz directa.
- $(A \odot C_\text{luz} \cdot \text{bright})$ es el color de la luz directa, modulado por el albedo (color propio) del material y la intensidad de la luz.
- $\max(0, \langle N, L̂ \rangle)$ es el coseno del ángulo entre la normal y la dirección a la luz, que determina cuánta luz incide realmente en la superficie (máximo cuando la luz pega de frente, cero si la luz viene de atrás).

Así, la fórmula suma la contribución ambiental (constante) y la directa (que depende de la orientación de la superficie respecto a la luz).

### Pasos concretos en el código

1. Intersección de escena (`intersect_scene`)
	- Recorre la lista de objetos y guarda el hit “más cercano” (`best`).
	- Calcula y devuelve: `ok`, `t`, `p` (P), `n` (N) y `albedo`.
	- Invierte la normal si `dot(N, r.dir) > 0` para garantizar que N mire al rayo entrante.

2. Sombreado difuso (`shade_lambert`)
	- Si no hay intersección: devuelve negro.
	- Calcula `ambient = ambient.color * ambient.ratio`.
	- Calcula `Ldir = normalize(light.pos - P)`.
	- Calcula `ndotl = max(0, dot(N, Ldir))`.
	- Calcula `diff = light.color * light.bright * ndotl`.
	- Devuelve `ambient + (albedo ⊙ diff)`.

3. Bucle de render (`render_scene`)
	- Para cada píxel, obtiene el rayo primario desde la cámara (`camera_build_frame` + `ray(...)`).
	- Llama a `intersect_scene` para obtener `hit` (hit info).
	- Llama a `shade_lambert(scene, &hit)` y escribe el color resultante en el framebuffer.

### Notas

- Normalización: Asegurarse de normalizar vectores de dirección y normales. Un `dot` con vectores no unitarios rompe la iluminación.
- Clampeo: `ndotl` debe truncarse a [0, 1]. Por debajo de 0 no hay contribución difusa (no hay “luz por detrás”).
- Orientación de normales: Invertir N cuando `dot(N, r.dir) > 0` evita zonas iluminadas mirando en la dirección equivocada.
- Rango de colores: Mantener componentes en [0,1] antes de convertir a RGBA evita artefactos. `vec3_to_rgba` ya hace el clamping final.
