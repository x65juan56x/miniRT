# COLOR_DISRUPTION.md

## Efecto Checkerboard en Ray Tracing: Explicación Didáctica

El efecto "checker" (damero o tablero de ajedrez) es una técnica clásica en gráficos por computadora para demostrar la capacidad de aplicar patrones espaciales sobre superficies 3D. En este documento se explica cómo se implementó el efecto checker en cada tipo de objeto de miniRT, abordando tanto los aspectos técnicos como los conceptos clave detrás de la técnica.

---

### 1. ¿Qué es el efecto checker?

El patrón checkerboard alterna dos colores en una cuadrícula regular, como un tablero de ajedrez. En 3D, esto significa que el color de un punto sobre la superficie depende de su posición espacial, alternando entre dos valores según una regla matemática.

**Analogía:** Imagina pintar un suelo con baldosas blancas y negras. Si caminas sobre él, tu pie estará sobre una baldosa blanca o negra dependiendo de dónde pises. En ray tracing, "pintamos" cada punto de la superficie según su posición.

---

### 2. Conceptos Clave

- **Coordenadas locales:** Para aplicar el patrón, primero se determina la posición del punto de impacto en el sistema de referencia del objeto.
- **Escalado:** El tamaño de los "cuadros" se controla con un parámetro de escala.
- **Función de paridad:** Se usa una función matemática (por ejemplo, `floor` y suma de índices) para decidir si el punto está en una casilla "clara" u "oscura".

**Ejemplo:** Si el punto de impacto tiene coordenadas (x, y), calculamos `floor(x / scale) + floor(y / scale)`. Si el resultado es par, usamos el color base; si es impar, el color alternativo.

---

### 3. Implementación Técnica por Objeto

#### a) Esfera (Sphere)

- **Proyección:** Se calcula la posición del punto de impacto en la esfera usando coordenadas esféricas (longitud y latitud).
- **Checker:** Se aplica el patrón sobre estas coordenadas, alternando colores según la suma de los índices de los "cuadros".
- **Ejemplo:** Como si la esfera fuera un globo terráqueo con líneas de latitud y longitud, y pintáramos los "cuadros" sobre la superficie.

#### b) Plano (Plane)

- **Proyección:** El plano tiene un sistema de coordenadas natural (x, y). Se toma el punto de impacto y se proyecta sobre el plano.
- **Checker:** Se calcula el índice de la casilla usando las coordenadas locales y la escala.
- **Ejemplo:** Como un suelo de baldosas, donde cada baldosa alterna color.

#### c) Triángulo (Triangle)

- **Proyección:** Se usan coordenadas baricéntricas o se proyecta el punto sobre el plano del triángulo.
- **Checker:** Se define una base local y se calcula el patrón checker sobre esa base.
- **Ejemplo:** Imagina un triángulo de cartulina sobre el que dibujas un patrón de damero.

#### d) Paraboloide Hiperbólico (Hyperbolic Paraboloid, HP)

- **Proyección:** El punto de impacto se proyecta sobre los ejes locales del paraboloide (u, v).
- **Checker:** Se calcula el patrón checker usando las coordenadas proyectadas y la escala.
- **Ejemplo:** Como si estiraras una tela cuadriculada sobre una silla de montar (forma típica del paraboloide hiperbólico).

---

### 4. Proceso General

1. **Intersección:** El ray tracing calcula el punto de impacto del rayo sobre el objeto.
2. **Transformación:** Se transforma ese punto a coordenadas locales del objeto.
3. **Cálculo de casilla:** Se determina en qué "cuadro" está el punto usando la escala y la función de paridad.
4. **Selección de color:** Se asigna el color base o alternativo según el resultado.

---

### 5. Ejemplo Práctico

Supón que tienes un plano en y=0 y quieres un checker de tamaño 1.0. Si el punto de impacto es (2.3, 0, 4.7):
- Calculas `floor(2.3 / 1.0) + floor(4.7 / 1.0) = 2 + 4 = 6`.
- Como 6 es par, usas el color base; si fuera impar, el alternativo.

---

### 6. Consideraciones y Detalles

- **Escala:** Permite ajustar el tamaño de los cuadros para cada objeto.
- **Coherencia:** El patrón checker se adapta a la geometría de cada objeto, respetando su forma y orientación.
- **Extensibilidad:** El mismo principio se puede aplicar a otros patrones (rayas, círculos, etc.) cambiando la función de decisión.

---

### 7. Conclusión

El efecto checkerboard es una excelente forma de visualizar la correcta proyección de coordenadas y la capacidad de aplicar texturas procedurales en ray tracing. La clave está en transformar el punto de impacto a un sistema de referencia adecuado y usar una función matemática sencilla para alternar colores.

---

