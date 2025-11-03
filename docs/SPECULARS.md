# Modelos de Especularidad en miniRT

En el renderizado 3D, la **luz especular** es la responsable de los brillos que vemos sobre los objetos, como el destello de una linterna sobre una esfera pulida. En miniRT hemos implementado tres modelos de especularidad: **Phong**, **Blinn-Phong** y **Reflective**. A continuación, explicamos cada uno de ellos de forma sencilla y didáctica.

---

## 1. Phong

**¿Qué es?**
El modelo de Phong es una forma clásica de simular el brillo especular en superficies. Imagina una bola de billar: cuando la luz la ilumina, ves un punto brillante que se mueve según tu posición. Phong calcula ese brillo comparando el reflejo perfecto de la luz con la dirección en la que miras.

**¿Cómo funciona?**
- Calcula el vector de reflejo de la luz sobre la superficie.
- Mide qué tan alineado está ese reflejo con tu ojo (el observador).
- Si están perfectamente alineados, el brillo es máximo; si no, disminuye rápidamente.

**Analogía:**
Piensa en lanzar una pelota contra el suelo: si la lanzas en el ángulo justo, rebota directo hacia ti (máximo brillo). Si cambias el ángulo, la pelota se aleja (menos brillo).

---

## 2. Blinn-Phong

**¿Qué es?**
Blinn-Phong es una variante del modelo de Phong, diseñada para ser más eficiente y estable en computadoras. En vez de usar el reflejo exacto, utiliza el "vector medio" entre la dirección de la luz y la dirección de tu vista.

**¿Cómo funciona?**
- Calcula el vector que está justo en medio entre la luz y tu ojo.
- Mide qué tan alineado está ese vector con la normal de la superficie.
- El brillo es máximo cuando la normal y ese vector medio apuntan igual.

**Analogía:**
Imagina que tú y la lámpara están en extremos opuestos de una mesa, y pones un espejo en el ángulo justo para que la luz rebote hacia ti. Blinn-Phong busca ese ángulo "óptimo" de forma más directa.

---

## 3. Reflective (Reflejos Recursivos)

**¿Qué es?**
El modelo Reflective no solo simula un brillo, sino que permite que los objetos actúen como espejos, reflejando su entorno. Así, puedes ver el reflejo de otros objetos en una superficie pulida.

**¿Cómo funciona?**
- Cuando un rayo de luz golpea una superficie reflectante, se genera un nuevo rayo en la dirección del reflejo.
- Ese nuevo rayo sigue su camino, pudiendo golpear otros objetos y así sucesivamente (hasta un límite de recursión).
- El color final es una mezcla entre el color local (Phong o Blinn) y el color que viene del reflejo.

**Analogía:**
Es como mirar en un espejo: ves no solo el brillo, sino todo lo que está frente al espejo, incluso otros reflejos.

---

## Similitudes y Diferencias

| Modelo        | ¿Simula brillo? | ¿Simula reflejo real? | ¿Eficiencia? | ¿Dónde se usa?           |
|--------------|-----------------|----------------------|--------------|--------------------------|
| Phong        | Sí              | No                   | Media        | Superficies plásticas    |
| Blinn-Phong  | Sí              | No                   | Alta         | Videojuegos, gráficos    |
| Reflective   | Sí + reflejo    | Sí                   | Baja         | Espejos, metales pulidos |

- **Phong y Blinn-Phong**: Ambos simulan el brillo especular, pero Blinn-Phong es más eficiente y estable en ángulos extremos.
- **Reflective**: Va más allá del brillo, permitiendo ver reflejos reales de otros objetos, pero es más costoso computacionalmente.

---

## Análisis detallado: Implementación de Blinn-Phong en miniRT

### ¿Dónde vive el Blinn-Phong en miniRT?
El modelo Blinn-Phong está implementado principalmente en la función `specular_blinn_phong` (archivo `src_bonus/shading/specular_bonus.c`). Esta función se encarga de calcular el brillo especular para un punto de la superficie cuando el material tiene seleccionado el modelo Blinn.

### ¿Cómo se activa?
Cada material en miniRT tiene un campo `model` (ver `t_material` en `material_bonus.h`) que puede ser `SPEC_MODEL_BLINN` o `SPEC_MODEL_PHONG`. Cuando el material de un objeto tiene el modelo Blinn, el sistema llama a `specular_blinn_phong` para calcular el brillo especular en ese punto.

### Flujo general del cálculo Blinn-Phong
1. **El rayo impacta un objeto:**
   - Cuando un rayo lanzado desde la cámara (o por reflexión) impacta un objeto, se genera un registro de impacto (`t_hit`).
2. **Se calcula la iluminación:**
   - Para cada luz de la escena, se evalúa si el punto está en sombra y se calculan los componentes difuso y especular.
3. **¿Qué modelo usar?**
   - Si el material tiene `model = SPEC_MODEL_BLINN`, se llama a `specular_blinn_phong`.
4. **Cálculo en `specular_blinn_phong`:**
   - Se obtienen los vectores necesarios:
     - **Normal** (`N`): perpendicular a la superficie en el punto de impacto.
     - **Dirección a la luz** (`L`): desde el punto hacia la luz.
     - **Dirección a la cámara** (`V`): desde el punto hacia el ojo/cámara.
     - **Vector medio** (`H`): la dirección que está justo en medio entre `L` y `V`.
   - Se calcula el ángulo entre la normal y el vector medio (`N·H`).
   - Se eleva ese valor a la potencia de la "brillantez" (`shininess`) para controlar la concentración del brillo.
   - Se multiplica por la intensidad de la luz y el coeficiente especular (`ks`).
   - El resultado es el color del brillo especular para esa luz.
5. **Se suma el resultado al color final del píxel.**

### Interacción con el resto del programa
- **Materiales:** Cada objeto tiene un material con parámetros que afectan el resultado:
  - `albedo`: color base del material.
  - `ks`: fuerza del brillo especular (qué tan "mojado" o "plástico" se ve).
  - `shininess`: controla qué tan concentrado es el brillo (valores altos = brillo pequeño y fuerte, valores bajos = brillo grande y suave).
  - `model`: elige entre Blinn o Phong.
- **Shading:** El sistema de shading (archivo `lambert_bonus.c`) llama a la función de specular adecuada según el modelo del material.
- **Luces:** El cálculo se repite para cada luz de la escena, sumando los brillos de todas.
- **Reflejos:** Si el material es reflectante, el color final es una mezcla entre el resultado local (incluyendo Blinn) y el color reflejado.

### Parámetros y su efecto visual
- **ks (coeficiente especular):**
  - Rango típico: 0.0 (sin brillo) a 1.0 (máximo brillo).
  - Visualmente: controla la intensidad del brillo especular. Un ks alto hace que el material parezca más pulido o mojado.
- **shininess (brillantez):**
  - Rango típico: 1 (muy difuso) a 1000+ (muy concentrado).
  - Visualmente: valores bajos producen brillos grandes y suaves (plástico viejo), valores altos producen brillos pequeños y definidos (metal pulido).
- **model:**
  - `SPEC_MODEL_BLINN` activa el cálculo Blinn-Phong.
  - `SPEC_MODEL_PHONG` activa el cálculo Phong clásico.

### Ejemplo de flujo (simplificado)
1. El usuario define un material en la escena:
   ```
   ks 0.8 200 blinn
   ```
2. El parser asigna estos valores al material del objeto.
3. Cuando un rayo impacta ese objeto, el shading detecta que el modelo es Blinn y llama a `specular_blinn_phong`.
4. Se calculan los vectores, el ángulo, y se obtiene el brillo especular.
5. El resultado se suma al color del píxel, junto con el difuso y el ambiente.

### ¿Por qué Blinn-Phong?
- Es más eficiente y estable que Phong, especialmente en ángulos extremos.
- Produce brillos visualmente agradables y es estándar en gráficos en tiempo real.

### Resumen visual
- **ks**: ¿Qué tan fuerte es el brillo?
- **shininess**: ¿Qué tan concentrado es el brillo?
- **model**: ¿Qué fórmula usar para el brillo?

Así, el modelo Blinn-Phong en miniRT permite simular materiales realistas y flexibles, controlando el aspecto del brillo de manera intuitiva y eficiente.


---

## 4. Análisis profundo de la implementación de Phong en miniRT

### ¿Qué es el modelo Phong?
El modelo de Phong es una técnica clásica para simular el brillo especular en superficies. Calcula la intensidad del brillo observando qué tan alineada está la dirección de visión con el reflejo perfecto de la luz sobre la superficie. Si el observador mira justo en la dirección del reflejo, el brillo es máximo; si no, disminuye rápidamente.

### Lógica y flujo de la implementación en miniRT

La función principal que implementa el modelo Phong en miniRT es:

```c
t_vec3 specular_phong(const t_scene *scene, const t_light *light, const t_hit *hit, t_vec3 light_dir);
```

#### Parámetros de la función
- `scene`: Puntero a la escena completa. Se usa para obtener la posición de la cámara (el "ojo" del observador).
- `light`: Puntero a la luz que está iluminando el punto. Se usa para obtener la intensidad y color de la luz.
- `hit`: Información sobre el punto de intersección en la superficie (normal, posición, material, etc.).
- `light_dir`: Vector unitario que apunta desde el punto de intersección hacia la luz.

#### Flujo paso a paso
1. **Validaciones iniciales**: Si alguno de los punteros es nulo, o si el material no tiene componente especular (`ks <= 0` o `shininess <= 0`), o la luz está apagada, retorna color negro (sin brillo).
2. **Cálculo de la dirección de visión**: Se calcula el vector desde el punto de intersección (`hit->p`) hacia la cámara (`scene->camera.pos`). Se normaliza para obtener la dirección de visión.
3. **Cálculo del vector de reflejo**: Se calcula el vector de reflejo de la luz sobre la superficie usando la normal en el punto (`hit->n`). Este vector representa la dirección en la que la luz "rebotaría" perfectamente.
4. **Cálculo del ángulo especular**: Se calcula el coseno del ángulo entre la dirección de visión y el vector de reflejo usando el producto punto. Si este valor es menor o igual a cero, significa que el observador no está en la zona del brillo y retorna negro.
5. **Cálculo de la intensidad especular**: Se eleva el coseno del ángulo a la potencia de `shininess` (el exponente de Phong). Un valor alto de `shininess` produce brillos pequeños y concentrados; un valor bajo, brillos grandes y difusos.
6. **Cálculo del color final**: Se multiplica la intensidad especular por la constante especular del material (`ks`), la intensidad de la luz y el color de la luz. El resultado es el color del brillo especular en ese punto.

#### Interacción con el resto del programa
- El resultado de `specular_phong` se suma al color final del píxel junto con los componentes difuso y ambiente.
- El modelo Phong se selecciona por material: cada material puede usar Phong o Blinn-Phong según su configuración.
- El flujo general es: el renderizador lanza un rayo, encuentra la intersección (`hit`), y luego llama a la función de sombreado, que a su vez llama a la función especular correspondiente.

#### ¿Qué representa cada parámetro visualmente?
- **`ks`**: Controla la intensidad del brillo especular. Si es 0, no hay brillo; si es 1, el brillo es máximo.
- **`shininess`**: Controla la "dureza" del brillo. Valores altos (ej: 1000) producen brillos pequeños y nítidos (como un metal pulido). Valores bajos (ej: 10) producen brillos grandes y suaves (como plástico).
- **`light_dir`**: Cambia la posición del brillo según la ubicación de la luz.
- **`scene->camera.pos`**: Cambia la posición del brillo según el punto de vista del observador.

---

## 5. Comparación detallada: Phong vs Blinn-Phong en miniRT

Ambos modelos buscan simular el brillo especular, pero difieren en cómo calculan la alineación entre la luz, la superficie y el observador.

### Similitudes
- **Propósito**: Ambos calculan un término especular para sumar al color final.
- **Parámetros**: Usan los mismos parámetros principales (`ks`, `shininess`, color/intensidad de la luz, normal de la superficie, dirección de la luz y del observador).
- **Estructura del flujo**: Validan entradas, calculan direcciones relevantes, computan un ángulo, elevan a la potencia de `shininess` y multiplican por la intensidad y color de la luz.

### Diferencias clave
| Paso | Phong | Blinn-Phong |
|------|-------|-------------|
| **Vector clave** | Usa el **vector de reflejo** de la luz sobre la normal (`reflect_dir`) | Usa el **vector bisector** (halfway) entre la luz y la vista (`half_vec`) |
| **Ángulo** | Calcula el ángulo entre el reflejo y la vista: <br> $\cos(\theta) = \vec{R} \cdot \vec{V}$ | Calcula el ángulo entre la normal y el bisector: <br> $\cos(\theta) = \vec{N} \cdot \vec{H}$ |
| **Cálculo del vector** | `reflect_dir = 2 * (N·L) * N - L` <br> (en miniRT: se calcula con la normal y la dirección de la luz) | `half_vec = normalize(L + V)` <br> (en miniRT: suma y normaliza la dirección de la luz y la vista) |
| **Sensibilidad angular** | El brillo máximo ocurre cuando el observador está exactamente en la dirección del reflejo | El brillo máximo ocurre cuando la normal está alineada con el bisector entre luz y vista |
| **Eficiencia** | Requiere calcular el vector de reflejo (más costoso) | Solo suma y normaliza dos vectores (más eficiente) |
| **Resultado visual** | El brillo se "mueve" más rápido y es más sensible a la posición del observador | El brillo es más estable y realista para materiales plásticos y metálicos |

### ¿Por qué elegir uno u otro?
- **Phong**: Útil para entender el modelo clásico y para materiales donde el brillo debe ser muy dependiente del ángulo de visión.
- **Blinn-Phong**: Más eficiente y produce brillos más suaves y realistas en la mayoría de los casos. Es el estándar en gráficos en tiempo real.

### Ejemplo de código (simplificado)

**Phong:**
```c
// Vector de reflejo
t_vec3 reflect_dir = reflect(-light_dir, normal);
float spec_angle = dot(view_dir, reflect_dir);
if (spec_angle > 0)
  spec = ks * pow(spec_angle, shininess);
```

**Blinn-Phong:**
```c
// Vector bisector (halfway)
t_vec3 half_vec = normalize(light_dir + view_dir);
float spec_angle = dot(normal, half_vec);
if (spec_angle > 0)
  spec = ks * pow(spec_angle, shininess);
```

### Resumen visual
- **Phong**: El brillo "baila" más con el movimiento de la cámara.
- **Blinn-Phong**: El brillo es más estable y natural.

---

## 6. Análisis profundo de la implementación de Reflective (Reflejos Recursivos) en miniRT

### ¿Qué es el modelo Reflective y por qué es recursivo?

El modelo Reflective permite que las superficies actúen como espejos, reflejando su entorno de manera física y realista. A diferencia de Phong y Blinn-Phong que solo simulan un brillo, Reflective calcula el color real que "llega" al ojo tras rebotar en la superficie.

**¿Por qué es recursivo?**
Es recursivo porque un rayo reflejado puede golpear otra superficie reflectante, que a su vez genera otro rayo reflejado, y así sucesivamente. Cada "rebote" requiere lanzar un nuevo rayo y calcular qué color trae, lo cual es exactamente lo que hace el algoritmo inicial. Por lo tanto, la función se llama a sí misma (recursión) hasta que:
1. Se alcanza el límite de profundidad (para evitar infinitos rebotes).
2. Se golpea un material no reflectante.
3. El rayo no golpea nada (escapa de la escena).

**Analogía física:**
Imagina dos espejos enfrentados: cuando miras uno, ves el reflejo del otro espejo, que a su vez refleja el primero, creando un túnel infinito de reflejos. En miniRT limitamos este "túnel" con un parámetro de profundidad para controlar cuántos rebotes permitimos.

---

### Lógica y flujo de la implementación en miniRT

La recursividad está implementada en dos funciones clave en `render_bonus.c`:

```c
static t_vec3 shade_surface(const t_scene *scene, const t_hit *hit, 
                            t_ray incoming, int depth);
static t_vec3 trace_recursive(const t_scene *scene, t_ray r, int depth);
```

#### Función `shade_surface`: El corazón de la recursión

Esta función calcula el color de una superficie considerando tanto el color local (difuso + especular) como el color reflejado.

**Parámetros:**
- `scene`: La escena completa con todos los objetos, luces y configuración.
- `hit`: Información del punto de impacto (posición, normal, material, etc.).
- `incoming`: El rayo que llegó a este punto (necesario para calcular la dirección de reflexión).
- `depth`: Profundidad de recursión actual. Empieza con un valor (ej: 5) y disminuye en cada rebote.

**Flujo paso a paso:**

```c
static t_vec3 shade_surface(const t_scene *scene, const t_hit *hit,
		t_ray incoming, int depth)
{
	float		kr;
	t_vec3		local;
	t_vec3		ref_dir;
	float		dir_len2;
	float		bias;
	t_vec3		reflect_col;
	float		mix_local;
	float		mix_reflect;

	// 1. Calcular el color local (difuso + especular + ambiente)
	local = shade_lambert(scene, hit);
	
	// 2. Obtener el coeficiente de reflectividad del material
	kr = clamp01(hit->reflectivity);
	
	// 3. CONDICIÓN DE TERMINACIÓN: ¿Seguir reflejando?
	if (depth <= 0 || kr <= 0.0f)
		return (local);  // No más rebotes, retornar solo color local
	
	// 4. Calcular la dirección del rayo reflejado
	// Fórmula: R = I - 2(N·I)N
	// Donde I = dirección del rayo entrante, N = normal
	ref_dir = v3_sub(incoming.dir,
		v3_mul(hit->n, 2.0f * v3_dot(incoming.dir, hit->n)));
	
	// 5. Normalizar la dirección de reflexión
	dir_len2 = v3_len2(ref_dir);
	if (dir_len2 < 1e-10f)
		return (local);  // Vector degenerado, no reflejar
	ref_dir = v3_mul(ref_dir, 1.0f / sqrtf(dir_len2));
	
	// 6. Calcular bias para evitar auto-intersección
	// El nuevo rayo debe empezar "ligeramente" alejado de la superficie
	bias = fmaxf(1e-4f, 1e-3f * hit->t);
	
	// 7. ¡AQUÍ OCURRE LA RECURSIÓN!
	// Lanzar un nuevo rayo en la dirección de reflexión
	// Nota: depth - 1 disminuye la profundidad
	reflect_col = trace_recursive(scene,
		ray(v3_add(hit->p, v3_mul(hit->n, bias)), ref_dir), depth - 1);
	
	// 8. Mezclar color local y color reflejado según kr
	mix_local = 1.0f - kr;
	mix_reflect = kr;
	return (v3_add(v3_mul(local, mix_local),
		v3_mul(reflect_col, mix_reflect)));
}
```

**¿Qué representa cada parámetro visualmente?**

- **`depth`**: Es el número de "rebotes" que el rayo puede hacer todavía. Cada llamada recursiva lo disminuye en 1.
  - `depth = 3`: Puedo ver hasta 3 reflejos encadenados (reflejo de un reflejo de un reflejo).
  - `depth = 0`: Ya no se calculan más reflejos, solo el color local.
  
- **`kr` (reflectivity)**: Controla qué tan "espejo" es el material.
  - `kr = 0.0`: Material opaco, no refleja nada (solo color local).
  - `kr = 1.0`: Espejo perfecto, solo se ve el reflejo (no el color local).
  - `kr = 0.5`: Mezcla 50% color local + 50% reflejo.

- **`ref_dir`**: La dirección en la que "rebota" el rayo.
  - Calculado con la ley de reflexión: el ángulo de incidencia = ángulo de reflexión.
  - Físicamente es como lanzar una pelota contra una pared: rebota simétricamente.

- **`bias`**: Un pequeño desplazamiento para evitar que el rayo se "auto-intersecte" con la misma superficie.
  - Sin bias: el rayo reflejado podría golpear inmediatamente el mismo punto, creando artefactos.
  - Con bias: el rayo empieza ligeramente "flotando" sobre la superficie.

---

#### Función `trace_recursive`: El puente de la recursión

Esta función es simple pero crucial: lanza un rayo y determina si golpea algo.

```c
static t_vec3 trace_recursive(const t_scene *scene, t_ray r, int depth)
{
	t_hit	hit;

	// Buscar la intersección más cercana
	if (!scene_hit(scene, r, FLT_MAX, &hit) || !hit.ok)
		return (v3(0.0f, 0.0f, 0.0f));  // No golpeó nada = negro
	
	// Golpeó algo: calcular su color (llamando a shade_surface)
	return (shade_surface(scene, &hit, r, depth));
}
```

**Papel en la recursión:**
- Recibe un rayo reflejado.
- Busca qué golpea.
- Si golpea algo, llama a `shade_surface` (que puede generar más rayos reflejados).
- Si no golpea nada, retorna negro (el rayo "escapa" de la escena).

---

### ¿En qué momento se produce la recursividad?

La recursividad ocurre exactamente en esta línea de `shade_surface`:

```c
reflect_col = trace_recursive(scene,
    ray(v3_add(hit->p, v3_mul(hit->n, bias)), ref_dir), depth - 1);
```

**Secuencia de llamadas:**

1. **Inicio:** `trace_pixel` lanza el rayo desde la cámara con `depth = scene->reflection_depth` (ej: 5).
2. **Primera intersección:** Golpea una esfera reflectante, llama a `shade_surface(depth=5)`.
3. **Primera reflexión:** `shade_surface` calcula el rayo reflejado y llama a `trace_recursive(depth=4)`.
4. **Segunda intersección:** El rayo reflejado golpea un plano reflectante, `trace_recursive` llama a `shade_surface(depth=4)`.
5. **Segunda reflexión:** `shade_surface` genera otro rayo reflejado y llama a `trace_recursive(depth=3)`.
6. ... y así sucesivamente hasta que `depth = 0` o el material no sea reflectante.

---

### ¿Qué representa cada iteración (nivel de recursión)?

Cada iteración representa un "rebote" del rayo de luz en la escena:

| Iteración | Depth | ¿Qué representa? | Ejemplo visual |
|-----------|-------|------------------|----------------|
| 1ª (inicial) | 5 | Rayo desde la cámara golpea el primer objeto | Veo una esfera plateada |
| 2ª (primer reflejo) | 4 | El rayo rebota en la esfera y golpea un segundo objeto | En la esfera veo el reflejo de un cubo rojo |
| 3ª (segundo reflejo) | 3 | El rayo rebota en el cubo y golpea un tercer objeto | En el reflejo del cubo veo el reflejo de un plano |
| 4ª (tercer reflejo) | 2 | El rayo rebota en el plano y golpea un cuarto objeto | En el plano reflejado veo otra esfera |
| ... | ... | ... | ... |
| Final | 0 | Ya no se calculan más reflejos | Solo se retorna el color local |

**Analogía de la sala de espejos:**
- **Iteración 1:** Miras un espejo y ves tu reflejo.
- **Iteración 2:** En tu reflejo, ves el reflejo de otro espejo detrás de ti.
- **Iteración 3:** En ese segundo reflejo, ves el reflejo del primer espejo.
- **Iteración 4:** Y así sucesivamente, creando el efecto de "túnel infinito".

---

### Interacción con el resto del programa

```
┌─────────────────────────────────────────────────────────────┐
│                        render_scene                         │
│  (Para cada píxel de la pantalla)                           │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
                  ┌──────────────┐
                  │ trace_pixel  │ ← depth = scene->reflection_depth
                  └──────┬───────┘
                         │
                         ▼
                  ┌─────────────────┐
                  │ trace_recursive │ ← Busca intersección
                  └──────┬──────────┘
                         │
                    ¿Golpeó algo?
                         │
                ┌────────┴────────┐
               NO                 SÍ
                │                 │
                ▼                 ▼
         (Negro/Fondo)    ┌──────────────┐
                          │shade_surface │ ← Calcula color local
                          └──────┬───────┘
                                 │
                          ¿kr > 0 && depth > 0?
                                 │
                        ┌────────┴────────┐
                       NO                 SÍ
                        │                 │
                        ▼                 ▼
                (Solo local)   ┌───────────────────┐
                               │Calcular reflexión │
                               │y llamar           │
                               │trace_recursive    │ ← RECURSIÓN
                               │con depth-1        │    (depth - 1)
                               └───────────────────┘
                                       │
                                       ▼
                            (Repetir el proceso)
```

**Puntos clave de interacción:**

1. **Parser:** Lee el parámetro `R` del archivo `.rt` que define `reflection_depth` (cuántos rebotes máximo).
2. **Material:** Cada objeto tiene `reflectivity` (kr) que determina si es reflectante.
3. **Shading:** El color local (difuso + especular) se mezcla con el color reflejado según `kr`.
4. **Rendimiento:** Más profundidad = más caro computacionalmente (cada rebote multiplica el costo).

---

### Ejemplo de código completo (comentado)

```c
// Punto de entrada: renderizar un píxel
static t_vec3 trace_pixel(const t_scene *scene, t_ray r, int show_normals)
{
	t_hit	hit;

	// Buscar la primera intersección desde la cámara
	if (!scene_hit(scene, r, FLT_MAX, &hit) || !hit.ok)
		return (v3(0.0f, 0.0f, 0.0f));  // No golpeó nada
	
	// Modo debug: mostrar normales
	if (show_normals)
		return (v3_mul(v3_add(hit.n, v3(1.0f, 1.0f, 1.0f)), 0.5f));
	
	// Calcular color con reflejos (depth inicial = reflection_depth)
	return (shade_surface(scene, &hit, r, scene->reflection_depth));
}

// Calcula el color de una superficie (con reflejos)
static t_vec3 shade_surface(const t_scene *scene, const t_hit *hit,
		t_ray incoming, int depth)
{
	float		kr;
	t_vec3		local;
	t_vec3		ref_dir;
	t_vec3		reflect_col;

	// Paso 1: Color local (difuso + especular + ambiente)
	local = shade_lambert(scene, hit);
	
	// Paso 2: ¿Es reflectante?
	kr = clamp01(hit->reflectivity);
	
	// Paso 3: Condición de parada de la recursión
	if (depth <= 0 || kr <= 0.0f)
		return (local);  // No más reflejos
	
	// Paso 4: Calcular dirección de reflexión
	ref_dir = v3_sub(incoming.dir,
		v3_mul(hit->n, 2.0f * v3_dot(incoming.dir, hit->n)));
	ref_dir = v3_norm(ref_dir);
	
	// Paso 5: Lanzar rayo reflejado (RECURSIÓN AQUÍ)
	reflect_col = trace_recursive(scene,
		ray(v3_add(hit->p, v3_mul(hit->n, bias)), ref_dir),
		depth - 1);  // ← depth disminuye
	
	// Paso 6: Mezclar local y reflejo según kr
	return (v3_add(v3_mul(local, 1.0f - kr),
		v3_mul(reflect_col, kr)));
}

// Traza un rayo y encuentra qué golpea
static t_vec3 trace_recursive(const t_scene *scene, t_ray r, int depth)
{
	t_hit	hit;

	// Buscar intersección
	if (!scene_hit(scene, r, FLT_MAX, &hit) || !hit.ok)
		return (v3(0.0f, 0.0f, 0.0f));
	
	// Calcular color del punto de impacto (puede generar más reflejos)
	return (shade_surface(scene, &hit, r, depth));
}
```

---

### Ejemplo visual concreto

**Escena:** Una esfera plateada (kr=0.8) frente a una pared roja, con `reflection_depth = 3`.

**Flujo de cálculo para un píxel que ve la esfera:**

1. **Depth = 3:** Rayo desde cámara golpea esfera.
   - Color local esfera: gris + brillo especular = gris brillante
   - kr = 0.8 (muy reflectante)
   - Lanza rayo reflejado hacia la pared.

2. **Depth = 2:** Rayo reflejado golpea pared roja.
   - Color local pared: rojo difuso
   - kr = 0.0 (no reflectante)
   - No lanza más rayos, retorna rojo.

3. **Depth = 3 (continuación):** Mezcla en la esfera.
   - Color final = (gris brillante * 0.2) + (rojo * 0.8)
   - Resultado: esfera gris con tinte rojo intenso (reflejo de la pared).

**Si hubiera dos espejos enfrentados con kr=0.9 y depth=10:**
- Verías un túnel de reflejos que se desvanece gradualmente.
- Cada iteración multiplica el color por 0.9, creando un efecto de "fade out".

---

### Resumen visual de los parámetros clave

| Parámetro | Rango típico | Efecto visual | Analogía |
|-----------|--------------|---------------|----------|
| **reflectivity (kr)** | 0.0 - 1.0 | 0.0 = opaco, 1.0 = espejo perfecto | Qué tan "limpio" está el espejo |
| **reflection_depth** | 1 - 10 | Cuántos reflejos encadenados se ven | Cuántos espejos puedes ver dentro de espejos |
| **depth** (en función) | Disminuye en cada llamada | Controla la parada de la recursión | Contador regresivo de rebotes |
| **bias** | ~1e-4 | Evita auto-intersección | Separar ligeramente el rayo de la superficie |

---

### Conclusión

La implementación de reflejos recursivos en miniRT es una técnica elegante que simula el comportamiento físico de la luz rebotando entre superficies. La recursión es natural porque cada reflejo es simplemente "volver a hacer" el cálculo de trazado de rayos desde un nuevo punto y dirección.

**Ventajas:**
- Reflejos físicamente precisos.
- Código limpio y modular.
- Fácil de entender conceptualmente.

**Limitaciones:**
- Costoso computacionalmente (cada rebote multiplica el trabajo).
- Necesita limitar la profundidad para ser viable.
- Puede crear artefactos si el bias no se calcula correctamente.

Así, el modelo Reflective en miniRT convierte superficies ordinarias en espejos realistas, permitiendo efectos visuales complejos con una implementación sorprendentemente simple y elegante.

---

