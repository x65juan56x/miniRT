# BUMP MAPPING

### ¿Qué es un bump map?
Un **bump map** (o mapa de relieve) es una técnica para simular pequeñas irregularidades o relieves en la superficie de un objeto **sin modificar realmente su geometría**.  
En vez de cambiar la forma del objeto, se altera la **normal** en cada punto de la superficie durante el cálculo de la iluminación, haciendo que la luz reaccione como si hubiera “bultos” o “hendiduras”.

Analogía  
Imagina que tienes una hoja de papel lisa y le dibujas sombras con lápiz para que parezca arrugada.
La hoja sigue siendo plana, pero tu ojo percibe profundidad por cómo cambian las sombras.
El bump map hace lo mismo: engaña a la luz para que parezca que la superficie tiene textura.

---

### ¿Cómo funciona técnicamente?

1. **Mapa de alturas (bump map):**
	- Es una imagen (o función) donde cada píxel representa una “altura” relativa.
	- Normalmente, valores claros = zonas altas, valores oscuros = zonas bajas.

2. **Cálculo de la normal perturbada:**
	- En vez de usar la normal geométrica (la “real”), se calcula una nueva normal en cada punto, basada en cómo cambia la altura en el bump map alrededor de ese punto.
	- Esto se hace calculando la derivada (el gradiente) del mapa de alturas en las coordenadas de textura (u, v).
3. **Iluminación:**
	- Se usa la normal perturbada para calcular la luz en ese punto, lo que genera el efecto de relieve.

¿Por qué “alrededor de ese punto” y no “exactamente en ese punto”?  
Imagina que tienes un mapa de alturas, como una foto en blanco y negro donde cada pixel indica “qué tan alto” es ese punto.  
Para saber si la superficie está inclinada, plana o tiene una pendiente en un punto, **no basta con saber la altura en ese punto:** necesitas saber cómo cambia la altura si te mueves un poquito hacia los lados.

**Analogía:**  
Piensa en estar parado en una montaña.
- Si miras solo tus pies, sabes tu altura, pero no si la montaña sube o baja alrededor tuyo.
- Si miras a tu alrededor y ves que el terreno sube hacia la derecha y baja hacia la izquierda, sabes que hay una pendiente.

Por eso, para simular el relieve, calculamos cómo cambia la altura alrededor del punto, mirando puntos cercanos.

¿Qué es el gradiente? ¿Qué representa?  
El **gradiente** es un vector que indica en qué dirección la altura aumenta más rápido y cuánto aumenta.

- En matemáticas, el gradiente de una función f(u, v) es:
	- $grad(f) = (∂f/∂u, ∂f/∂v)$
	- Es decir, cuánto cambia f si te mueves un poquito en u, y cuánto cambia si te mueves en v.

**En bump mapping:**

- El bump map es una función de altura: $h(u, v)$
- El gradiente es: $(dh/du, dh/dv)$
- $dh/du$: cuánto cambia la altura si te mueves un poco en la dirección u.
- $dh/dv$: cuánto cambia la altura si te mueves un poco en la dirección v.

**Visualmente:**  
El gradiente apunta “cuesta arriba” en el mapa de alturas.

---

### Proceso paso a paso para implementar bump mapping

1. Obtener las coordenadas de textura (u, v)
- Para cada punto de intersección en la superficie, calcula sus coordenadas de textura (u, v).
- Ejemplo: en una esfera, u y v pueden derivarse de la posición angular del punto.
2. Leer el valor del bump map
- Accede al valor de altura en el bump map usando (u, v).
- Ejemplo: `height = bump_map[u][v]`
3. Calcular el gradiente (cómo cambia la altura)
- Calcula cómo varía la altura alrededor de (u, v):
	- `dhdu = height(u + delta, v) - height(u, v)`
	- `dhdv = height(u, v + delta) - height(u, v)`
- Esto te da la “pendiente” en cada dirección.
4. Construir la nueva normal
- Usa el gradiente para modificar la normal original.
- En coordenadas locales de la superficie, la nueva normal se calcula como:
	- `N' = N + scale * (dhdu * tangent_u + dhdv * tangent_v)`
	- Donde `tangent_u` y `tangent_v` son los vectores tangentes a la superficie en las direcciones u y v.
- Normaliza el resultado para obtener una normal válida.
5. Usar la normal perturbada en el shading
- En el cálculo de iluminación (Lambert, Phong, etc.), usa la nueva normal en vez de la geométrica.
- Esto hará que la luz y las sombras simulen el relieve.

---

### Ejemplo visual

Imagina una esfera lisa.
- Sin bump map: la luz se refleja suavemente, parece pulida.
- Con bump map: la luz se dispersa en patrones irregulares, parece rugosa o con detalles.

---

### Resumen de conceptos clave

- Bump map: Imagen de alturas, no cambia la geometría, solo la normal.
- Normal perturbada: Se calcula usando el gradiente del bump map.
- Iluminación: Se basa en la normal modificada, creando el efecto de relieve.
- No hay geometría nueva: El objeto sigue siendo igual de simple para el raytracer.

---

### Ejemplo de código (pseudo-C)

``` c
// Supón que tienes la normal geométrica N, y tangentes T_u, T_v
float h = bump_map(u, v);
float h_u = bump_map(u + delta, v);
float h_v = bump_map(u, v + delta);

float dhdu = h_u - h;
float dhdv = h_v - h;

vec3 N_perturbed = normalize(N + scale * (dhdu * T_u + dhdv * T_v));
```