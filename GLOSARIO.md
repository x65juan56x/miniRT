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
- Pixel sampling: convertir cada píxel en uno o varios rayos que salen de la cámara.
- Anti-aliasing: técnica para suavizar bordes dentados promediando varias muestras por píxel.

## 3) Objetos geométricos

- Esfera (sp): conjunto de puntos a una distancia fija (radio) de un centro.
- Plano (pl): superficie infinita definida por un punto y una normal (vector perpendicular a la superficie).
- Cilindro (cy): superficie generada alrededor de un eje con un radio y una altura. Puede tener tapas (discos) arriba y abajo.
- Cono (bonus): superficie cuyo radio varía linealmente con la altura; tiene un vértice y un eje.
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

## 7) Parsing del archivo `.rt`

- Token: fragmento de texto separado por espacios o comas que representa un dato (ej. "255,0,0").
- Identificador: letras que indican el tipo de elemento (A, C, L, sp, pl, cy, …).
- Normalización de input: manejo de espacios múltiples y líneas en blanco.
- Validación de rangos: comprobar que los números están en límites permitidos.
- Mensaje de error: texto claro que explica qué falló (línea, campo, motivo).
- Unicidad: elementos que deben aparecer solo una vez (como A, C, L en el mandatory).

## 8) Calidad, errores y estabilidad

- RAII-like en C: convención de tener funciones `create/destroy` por módulo para gestionar recursos.
- Estructuras de ownership: definir claramente quién libera qué y cuándo.
- Sanitizers: opciones del compilador que detectan errores en tiempo de ejecución (AddressSanitizer).
- Epsilon numérico: pequeño valor usado para evitar divisiones por cero y problemas de precisión con `double`.

## 9) Optimización y rendimiento

- Hot path: parte del código que se ejecuta con mayor frecuencia (crítica para el rendimiento), como bucles de píxeles.
- Cache locality: organizar datos para que el acceso en memoria sea eficiente.
- Branching: decisiones `if/else`; reducirlas en el hot path puede acelerar.
- Precomputación: calcular una vez valores que se reutilizan (ej. base de cámara).

## 10) Bonus y extensiones

- Checkerboard (tablero): textura procedural que alterna colores en un patrón cuadriculado.
- Phong completo: modelo con componentes ambiente, difusa y especular.
- Reflexión recursiva: rayos que rebotan en superficies reflectantes hasta cierta profundidad.
- Bump mapping: técnica que altera la normal aparente para simular relieve sin cambiar la geometría.
- Soft shadows: sombras con bordes suaves usando luces de área y muestreo.

## 11) Utilidades y formatos

- Clamp: limitar un valor a un rango (ej. 0–1 para colores).
- Lerp (interpolación lineal): mezcla entre dos valores según un factor t en [0–1].
- NaN/Inf: valores no numéricos o infinitos; deben detectarse y manejarse.
- Endianness: orden de bytes en memoria; puede importar al escribir píxeles.

## 12) Testing y escenas

- Escena mínima: archivo `.rt` con A, C, L y un objeto simple para validar pipeline.
- Escenas negativas: archivos mal formados para comprobar manejo de errores.
- Golden image: imagen de referencia para comparar resultados tras cambios.

---
