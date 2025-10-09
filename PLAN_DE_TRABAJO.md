# Plan de Trabajo miniRT (equipo de 2)

Este documento propone un plan gradual para desarrollar miniRT, incluyendo bonus, y define prácticas de calidad (errores, casos límite, leaks, segfaults) y una división de responsabilidades efectiva para un equipo de dos personas.

## Objetivos por fases

### Fase 0 — Preparación
- Acordar convenciones de código (nombres, normas, 80/Norminette, estilo de errores).
- Configurar Makefile con objetivos: `all clean fclean re bonus` y flags de desarrollo: `-Wall -Wextra -Werror`, opcional `-g`, `-fsanitize=address` para debug.
- Estructura inicial de carpetas:
  - `include/` headers públicos
  - `src/core/` motor de raytracing
  - `src/parse/` parser `.rt`
  - `src/mlx/` integración MiniLibX y eventos
  - `src/math/` álgebra vectorial
  - `src/geom/` intersecciones (sp, pl, cy, …)
  - `src/bonus/` extensiones opcionales
  - `assets/scenes/` escenas de prueba
  - `tests/` utilidades de test
- Integración de libft (si se usa) y MiniLibX.

### Fase 1 — Parser y validación
- Lectura robusta del archivo `.rt` línea a línea (soportar espacios múltiples y líneas en blanco).
- Tokenización segura y validaciones:
  - Rangos: colores [0–255], ratio [0–1], FOV [0–180], vectores en [-1,1].
  - Unicidad de A, C, L (si el subject lo requiere).
  - Normalización de vectores no nulos (error si norma ~0).
  - Conversión segura a double/int con detección de overflow/NaN.
- Mensajes de error claros: `Error\n<detalle>` y salida limpia (free de recursos).
- Escenas mínimas de prueba y escenas con errores intencionales en `assets/scenes/`.

### Fase 2 — Matemáticas y estructuras
- `vec3`: suma, resta, dot, cross, length, normalize (con tolerancias y guardas).
- Tipos: `t_ray`, `t_camera`, `t_color`, `t_light`, `t_material`, `t_object` (union para sp/pl/cy), `t_scene`.
- Utilidades: clamp, mix/lerp, casi-igual con epsilon.

### Fase 3 — Cámara y rayos
- Construir rayos primarios desde la cámara (FOV horizontal, aspect ratio, basis u/v/w ortonormal a partir de la orientación).
- Viewport y mapeo pixel→rayo, con anti-aliasing simple (N muestras opcional).

### Fase 4 — Intersecciones
- Esfera (cuadrática estable numéricamente, elegir t mínimo positivo > eps).
- Plano (evitar división por ~0: paralelo; normal orientada correctamente).
- Cilindro: lateral y tapas (caps), proyecciones y límites por altura; manejar rayos casi paralelos al eje; normal correcta.
- Estructura `t_hit` con `t`, `p`, `n`, `albedo`, puntero al objeto, etc.

### Fase 5 — Iluminación y sombras
- Ambient + difusa (Lambert) con clamp a [0,1].
- Sombras duras: shadow ray con bias para evitar acne (p + n*EPS).
- Múltiples luces (aunque el mandatory permita 1) para preparar bonus.

### Fase 6 — Integración MLX y UI
- Ventana, imagen, bucle de eventos.
- Inputs: ESC cierra; red cross cierra; opcional: cambiar cámara/escena.
- Render a buffer y volcado a imagen MLX.

### Fase 7 — Calidad, test y fixes
- Pruebas: escenas unitarias por objeto, por luz y por error.
- Valgrind (mem/leaks), AddressSanitizer (opcional), comprobación de dobles frees.
- Time profiling simple (gettimeofday) y micro-optimización de hot paths.

### Fase 8 — Bonus
- Reflexiones especulares (Phong): componente specular + shininess; opcional reflexión recursiva con depth limitado.
- Checkerboard procedural: en planos y/o esferas (patrón en espacio del objeto).
- Luces coloreadas y múltiples luces.
- Objeto adicional: cono (ecuación cuadrática, límites por altura y cap), u otra cuádrica.
- Opcionales si hay tiempo: bump mapping simple (normal perturbation), soft shadows (luces área por muestreo).

## División de responsabilidades (equipo de 2)

Sugerencia A/B con ownership claro y code reviews cruzadas.

### Persona A — Parsing y Core Geométrico
- Parser `.rt`, validaciones y mensajes de error.
- Estructuras de escena y carga de materiales/objetos.
- Intersecciones: esfera y plano.
- Infraestructura de testing del parser y escenas de error.

### Persona B — Render y Motor de Iluminación
- Integración MiniLibX y pipeline de render.
- Cámara, generación de rayos y muestreo.
- Intersección de cilindro (lateral + caps).
- Iluminación (ambient+difusa) y sombras.

### Bonus (distribución)
- A: Checkerboard, cono.
- B: Phong specular, luces coloreadas, múltiples luces.

Ambos: refactors, optimizaciones y revisión mutua. Pactar interfaces en `include/` para trabajar en paralelo.

## Gestión de errores, límites y estabilidad

### Reglas generales
- Nunca continuar con entrada inválida. Validar cada token/campo al parsear.
- Ante error: imprimir `Error\n<mensaje>` a stderr, liberar todo y `exit(1)`.
- Epsilon global (ej. `1e-6`) para comparaciones de doubles.
- Bias para evitar self-intersections en sombras/reflexiones (ej. `p + n*1e-4`).

### Casos límite a cubrir
- Vectores casi nulos en normal/orientación de cámara → error.
- FOV fuera de rango o 0/180 → error.
- Colores fuera de [0–255] o ratio fuera [0–1] → error.
- Rayos paralelos a plano/cilindro → sin intersección; evitar NaNs.
- Discriminante ~0 en esfera/cono → raíz doble; tomar t>eps.
- Intersecciones detrás de la cámara (t<0) → descartar.
- Cilindro: puntos fuera de altura; manejo correcto de caps.
- Overflow/underflow al convertir strings → error.

### Memoria y recursos
- Patrón de ownership claro: `scene_free(scene)` libera todo.
- RAII-like en C: funciones `*_create` y `*_destroy` por módulo.
- Evitar fugas en rutas de error: helpers `bool parse_* (.., t_error_ctx*)` que limpian al fallar.
- Valgrind en CI local: objetivo `make memcheck` que ejecute escenas.

### Prevención de segfaults
- Chequear punteros antes de usar.
- Inicializar estructuras con `memset(0)` o constructores.
- Indices/loops siempre validados.
- Defensive programming en math (comprobar divisor > eps).

## Calidad y pruebas

### Escenas recomendadas (`assets/scenes/`)
- `minimal.rt`: A, C, L, 1 sp
- `planes.rt`: varios planos con checkerboard (bonus)
- `cylinders.rt`: distintos ejes, alturas y caps
- `errors/*.rt`: casos malformados (rangos, formatos, duplicados)
- `lights.rt`: múltiples luces y colores (bonus)

### Automatización
- Objetivos Makefile:
  - `make dev` → compila con `-g -fsanitize=address`
  - `make release` → flags estrictos, sin sanitizer
  - `make memcheck` → corre valgrind sobre escenas ejemplo

### Criterios de aceptación (mandatory)
- Render en ventana MLX, ESC y close limpian y salen sin leaks.
- Soporte de A, C, L, sp, pl, cy con transformaciones y tamaños.
- Iluminación ambient + diffuse, sombras duras.
- Parser robusto: errores detectados y reportados limpiamente.

### Criterios de aceptación (bonus)
- Phong specular visible y configurable por material.
- Checkerboard aplicado y ajustable.
- Múltiples luces con color.
- Cono funcional con límites y normal correcta.

## Notas finales
- Documentar en `README.md` cómo correr escenas y atajos.
- Mantener funciones cortas y módulos con responsabilidad única.
- Revisiones cruzadas obligatorias antes de merges.
