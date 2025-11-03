# Glosario de miniRT

Este glosario explica los términos que aparecen en el subject de miniRT y en el plan de trabajo.

## 1) Geometría y álgebra mínima

- 3D: espacio con 3 ejes (x, y, z).
- Vector: flecha en el espacio. Se usa para posiciones, direcciones y normales. Ej.: (x, y, z).
- Punto vs. Vector: un punto indica una posición; un vector, una dirección/ desplazamiento.
- Norma (longitud): tamaño de un vector. `|v| = sqrt(x^2 + y^2 + z^2)`.
- Normalizar: convertir un vector a longitud 1 manteniendo su dirección.
- Producto escalar (dot): `a·b = |a||b|cos(θ)`. Sirve para medir alineación entre vectores.
- Producto vectorial (cross): da un vector perpendicular a dos vectores.
- Epsilon: número muy pequeño usado para evitar errores de redondeo (ej. 1e-6).

## 2) Ray Tracing (trazado de rayos)

- Rayo: línea dirigida definida por un origen y una dirección. Fórmula: `P(t) = origen + t * dirección`.
- Cámara: define desde dónde miramos y hacia dónde. Tiene posición, orientación y FOV.
- FOV (Field of View): ángulo de apertura de la cámara; controla cuánto "vemos".
- Viewport: rectángulo virtual (plano de imagen) donde se proyecta la escena.
- Focal length: Distancia desde el origen de los rayos al viewport.
- Pixel sampling: convertir cada píxel en uno o varios rayos que salen de la cámara.
- Anti-aliasing: técnica para suavizar bordes dentados promediando varias muestras por píxel. (Ver SSAA en este documento)

## 3) Objetos geométricos

- Esfera (sp): conjunto de puntos a una distancia fija (radio) de un centro.
- Plano (pl): superficie infinita definida por un punto y una normal (vector perpendicular a la superficie).
- Cilindro (cy): superficie generada alrededor de un eje con un radio y una altura. Puede tener tapas (discos) arriba y abajo.
- Diámetro: tamaño de una esfera (2 × radio) o de la base del cilindro.
- Altura (cilindro/cone): longitud del objeto a lo largo de su eje.

## 4) Intersecciones rayo-objeto

- Intersección: punto(s) donde un rayo choca con un objeto.
- t (parámetro): distancia escalar a lo largo del rayo; se busca el t positivo más pequeño.
- Discriminante: parte de la fórmula cuadrática que indica cuántas soluciones hay (negativo: ninguna; cero: tangente; positivo: dos).
- Paralelismo: cuando un rayo es paralelo a una superficie (como un plano), puede no intersectarla.
- Caps (tapas): discos que cierran un cilindro/cone limitado; requieren test adicional de intersección y límites.
- Normal de superficie: vector perpendicular en el punto de intersección; se usa para iluminar.
- Acne de sombras: artefacto cuando un rayo de sombra se auto-intersecta con la superficie de salida; se mitiga con un pequeño desplazamiento (bias).

## 5) Iluminación

- Color: combinación de componentes R, G, B. Puede representarse como [0–255] o [0–1].
- Albedo: color/reflectividad difusa base de un material (cuánto color refleja sin brillo).
    - Color difuso se refiere al color base de un material, es decir, el color que ves cuando la luz incide sobre la superficie y se dispersa en todas direcciones. Es el color “real” del objeto, sin reflejos ni brillos especulares. En ray tracing, este color suele llamarse albedo.
- Luz ambiental (ambient): luz base global que ilumina todo ligeramente (evita negro total).
- Luz puntual (spot/light): fuente de luz en una posición; emite luz en todas direcciones.
- Difusa (Lambert): componente que depende del ángulo entre la luz y la normal; máximo cuando la luz pega frontalmente.
- Especular (Phong) [bonus]: brillo que aparece cuando la vista se alinea con la reflexión de la luz; simula reflejos pulidos.
- Múltiples luces [bonus]: varias fuentes de luz que se suman, cada una con color e intensidad.
- Sombra dura: cuando un objeto bloquea completamente la luz entre el punto y la fuente.

## 6) Cámara, transformaciones y coordenadas

- Orientación: hacia dónde apunta la cámara u objeto (vector normalizado).
- Traslación: mover un objeto o la cámara en el espacio.
- Rotación: girar un objeto alrededor de un eje; para la cámara y cilindros/planos es relevante.
- Sistema de coordenadas local: eje propio de un objeto/cámara, útil para cálculos.
- Base ortonormal: tres vectores perpendiculares y normalizados (u, v, w) que forman un sistema de referencia.
- Ortogonal (orthogonal): En matemáticas y gráficos, significa “perpendicular”. Dos vectores son ortogonales si su producto escalar (dot product) es cero, es decir, forman un ángulo de 90° entre sí.

## 7) Bonus y extensiones

- Checkerboard (tablero): textura procedural que alterna colores en un patrón cuadriculado.
- Phong completo: modelo con componentes ambiente, difusa y especular.
- Reflexión recursiva: rayos que rebotan en superficies reflectantes hasta cierta profundidad.
- Bump mapping: técnica que altera la normal aparente para simular relieve sin cambiar la geometría.

## 8) Utilidades y formatos

- Clamp: limitar un valor a un rango (ej. 0–1 para colores).
- Lerp (interpolación lineal): mezcla entre dos valores según un factor t en [0–1].
- NaN/Inf: valores no numéricos o infinitos; deben detectarse y manejarse.
- Endianness: orden de bytes en memoria; puede importar al escribir píxeles.

## 9) Testing y escenas

- Escena mínima: archivo `.rt` con A, C, L y un objeto simple para validar pipeline.
- Escenas negativas: archivos mal formados para comprobar manejo de errores.
- Golden image: imagen de referencia para comparar resultados tras cambios.

## 10) Coordenadas UV

¿Qué son las coordenadas UV?

Las **coordenadas UV** son un sistema de coordenadas 2D normalizado que mapea cualquier superficie 3D a un espacio de textura 2D. El nombre viene de las letras U y V (para diferenciarse de X, Y, Z del espacio 3D).

Características fundamentales:
- **Rango:** Típicamente [0,1] en ambas direcciones U y V
- **U:** Coordenada horizontal (equivalente a X en 2D)
- **V:** Coordenada vertical (equivalente a Y en 2D)
- **Independientes del tamaño:** Normalizadas, funcionan con cualquier resolución

¿Para qué sirven?
1. **Mapeo de texturas:** Aplicar imágenes 2D sobre superficies 3D
2. **Mapeo de píxeles:** Convertir coordenadas de pantalla a espacio normalizado
3. **Sampling:** Tomar muestras uniformes de una superficie
4. **Interpolación:** Calcular valores intermedios entre vértices
5. **Procedural textures:** Generar patrones basados en posición

¿Qué representan en nuestro código?

En `generate_background()`:
``` c
float u = (float)x / (float)(width - 1);  // [0,1] horizontal
float v = (float)(height - 1 - y) / (float)(height - 1);  // [0,1] vertical
```

**u** representa:

- 0.0 = borde izquierdo de la pantalla
- 0.5 = centro horizontal
- 1.0 = borde derecho de la pantalla

**v** representa:

- 0.0 = parte inferior de la pantalla (Y invertida)
- 0.5 = centro vertical
- 1.0 = parte superior de la pantalla

¿Cómo se usan en ray tracing?
1. Mapeo píxel → viewport:
```c
// lower_left + u*horizontal + v*vertical
t_vec3 dir = v3_add(lower_left, v3_add(v3_mul(horizontal, u), v3_mul(vertical, v)));
```
2. Interpolación bilinear:
- u=0,v=0 → esquina inferior izquierda del viewport
- u=1,v=0 → esquina inferior derecha
- u=0,v=1 → esquina superior izquierda
- u=1,v=1 → esquina superior derecha

¿Cuándo son necesarias?

**En Ray Tracing:**
- Generación de rayos primarios: Cada píxel → coordenadas UV → dirección de rayo
- Anti-aliasing: Múltiples muestras UV por píxel para suavizar
- Texturas: Mapear texturas 2D sobre objetos 3D
- Materiales procedurales: Checkerboard, gradientes, ruido

**En Computer Graphics general:**
- UV unwrapping: Desplegar modelos 3D en coordenadas 2D
- Texture atlases: Combinar múltiples texturas en una imagen
- Lightmapping: Precalcular iluminación en coordenadas UV
- Normal mapping: Alterar normales según textura

Inversión de coordenada V  
Nota el `(height - 1 - y)` en nuestro código:
```c
float v = (float)(height - 1 - y) / (float)(height - 1);
```
**¿Por qué?**
- Sistema de ventana: Y=0 arriba, Y=height-1 abajo
- Sistema matemático: V=0 abajo, V=1 arriba
- Inversión necesaria: Para que V=0 corresponda al fondo del gradiente

Ejemplos prácticos de uso:
1. **Textura de tablero de ajedrez:**
```c
int checker_u = (int)(u * 8) % 2;
int checker_v = (int)(v * 8) % 2;  
bool is_white = (checker_u + checker_v) % 2 == 0;
```
2. **Gradiente circular:**
```c
float center_u = u - 0.5f;
float center_v = v - 0.5f;
float distance = sqrtf(center_u*center_u + center_v*center_v);
```
3. **Mapeo esférico (para skybox):**
```c
float theta = atan2f(dir.z, dir.x);  // [-π, π]
float phi = asinf(dir.y);            // [-π/2, π/2]
float u = (theta + PI) / (2*PI);     // [0, 1]
float v = (phi + PI/2) / PI;         // [0, 1]
```

Las coordenadas UV son fundamentales en ray tracing porque proporcionan una forma uniforme y escalable de mapear entre espacios 2D y 3D, permitiendo efectos visuales complejos de manera matemáticamente elegante.

### ¿Pierdo precisión usando coordenadas uv normalizadas?

No. Cuando usas coordenadas $u$ y $v$ en el rango $[0,1]$ para mapear píxeles a posiciones en el plano de imagen, lo que haces es normalizar la posición del píxel respecto a la resolución:

- $u = \frac{x + 0.5}{\text{width}}$ (con $x$ en $[0, \text{width}-1]$)
- $v = \frac{y + 0.5}{\text{height}}$ (con $y$ en $[0, \text{height}-1]$)

Esto **no implica pérdida de precisión relevante** en la práctica, porque:

- El número de valores distintos de $u$ y $v$ es exactamente igual al número de píxeles en cada eje (por ejemplo, 1920 y 1080).
- Los floats de 32 bits pueden representar exactamente todos los enteros hasta $2^{24}$, y los pasos entre $u$ adyacentes son $1/\text{width}$, que es perfectamente representable para resoluciones normales.
- El cálculo de la dirección del rayo y la posición en el plano de imagen se hace con precisión suficiente para cualquier resolución de pantalla típica (hasta 8K o más).

**¿Por qué no escalar el sistema en función de la resolución?**
- El sistema de $u,v$ en $[0,1]$ desacopla la lógica de la cámara de la resolución: puedes cambiar la resolución y la proyección no cambia.
- Si escalaras todo en función de la resolución, tendrías que ajustar el tamaño del plano de imagen y la cámara cada vez que cambias la ventana, lo que complica el código y puede introducir distorsiones.
- El mapeo $u,v$ es estándar en gráficos y ray tracing porque es simple, robusto y portable.

**En resumen:**
No hay pérdida de precisión significativa usando $u,v$ en $[0,1]$ para resoluciones normales. Es la forma más limpia y flexible de mapear píxeles a rayos, y no necesitas escalar el sistema en función de la resolución.


---

## SSAA - Super Sampling Anti-Aliasing

Es una técnica de anti-aliasing (suavizado de bordes) en gráficos por computadora y ray tracing. Consiste en renderizar cada píxel de la imagen a una resolución mayor (por ejemplo, 2× o 4× más muestras por píxel), promediando los colores de esas submuestras para obtener el color final del píxel.

### ¿Cómo funciona en ray tracing?

- Por cada píxel, lanzas varios rayos ligeramente desplazados dentro del área del píxel (no solo en el centro).
- Calculas el color de cada rayo.
- Haces el promedio de todos esos colores y lo usas como color final del píxel.

### Ventaja:

Reduce el aliasing (bordes dentados) y produce imágenes más suaves y realistas.

### Ejemplo:

Para 4× SSAA, lanzas 4 rayos por píxel (en una cuadrícula 2×2 dentro del píxel), sumas los colores y divides por 4.

### Resumen:

SSAA = Super Sampling Anti-Aliasing = suavizado de bordes por muestreo múltiple y promedio por píxel.
