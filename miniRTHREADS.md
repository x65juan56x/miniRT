# miniRTHREADS: Render multihilo en miniRT (versión bonus)

Este documento describe, de forma conceptual y técnica, la implementación del renderizado multihilo en la versión bonus de miniRT: flujo, funciones, decisiones de diseño, cómo se evita deadlock/data races, y detalles de división del trabajo.

Índice
- Objetivos y alcance
- Diseño general
- Flujo de ejecución
- Funciones y responsabilidades
- División del trabajo (pantalla → hilos)
- Sincronización y seguridad de datos
- Uso de pthreads (qué y por qué)
- Gestión de errores y fallbacks
- Consideraciones de rendimiento y mejoras futuras
- Depuración y verificación

---

## Objetivos y alcance

- Acelerar el render utilizando múltiples CPU cores.
- Evitar data races y artefactos (ruido/bandas) sin introducir deadlocks.
- Mantener la ruta monohilo como fallback.
- Cero cambios en la lógica de shading/intersección; el paralelismo es envolvente.

Alcance: solo versión bonus. MLX y el bucle principal siguen en el hilo principal.

---

## Diseño general

- Pool ad hoc: se crean N−1 hilos de trabajo por render; el hilo principal también trabaja (total N hilos).
- Partición por filas: cada hilo procesa un bloque contiguo de filas [y_start, y_end).
- Sin bloqueos: sin mutex/condvars. Se evitan data races:
  - Framebuffer: regiones exclusivas por hilo (filas no solapadas).
  - Escena: cada hilo usa una copia privada (scene_clone) para evitar escrituras concurrentes sobre campos “vars” de objetos.
  - Parámetros de cámara: copia por tarea (solo lectura).
- Fallback: si algo falla (memoria/hilos), se renderiza en monohilo.

---

## Flujo de ejecución

Archivo clave: `src_bonus/render/render_bonus.c`

1. `render_scene(app)`
   - Construye el frame de cámara (`camera_build_frame`).
   - Determina `thread_count` efectivo (`clamp_thread_count`).
   - Si `thread_count == 1` → `render_scene_single_thread(app, &frame)`.
   - Si multihilo:
     - Reserva arrays para `pthread_t` y `t_render_task` (`alloc_workers`).
     - Crea copias de la escena (`scene_clone`) para cada hilo.
     - Llama a `fill_tasks(...)` para asignar filas por hilo.
     - Lanza `spawn_count = thread_count - 1` hilos con `pthread_create(...)` (`launch_workers`).
     - Ejecuta el último task en el hilo principal (evita estar ocioso).
     - Espera a los hilos (`join_workers`).
     - Libera clones y estructuras temporales.

2. Cada hilo ejecuta `render_worker(task)`:
   - Recorre sus filas [y_start, y_end).
   - Para cada píxel:
     - Calcula (u,v), construye el rayo, traza y escribe en el framebuffer global en su bloque exclusivo.

3. Al finalizar (fuera de este archivo), el frame se sube a la textura MLX en el hilo principal.

---

## Funciones y responsabilidades

- `camera_build_frame(...)` (bonus): construye `t_cam_frame` (origin, lower_left, horizontal, vertical). Solo lectura.

- `render_scene(t_app *app)`
  - Orquesta el render por frame y elige mono/multihilo.
  - Llama a helpers de asignación/creación/unión/liberación.

- `clamp_thread_count(const t_app *app)`
  - Garantiza límites razonables:
    - ≥ 1, ≤ MAX_RENDER_THREADS (16).
    - No más hilos que filas (evita hilos sin trabajo).

- `alloc_workers(pthread_t **threads, t_render_task **tasks, int thread_count)`
  - Reserva estructuras para hilos y tareas.
  - Nota: crea `spawn_count = thread_count - 1` `pthread_t`. El último task se ejecuta en el hilo principal.

- `scene_clone(const t_scene *src)` / `scene_clone_free(t_scene *scene)`
  - Crea una copia superficial de la escena:
    - Copias de nodos `t_object` (lista enlazada), pero comparten punteros internos inmutables (texturas, etc.).
    - Evita que distintos hilos modifiquen el mismo `t_object` (especialmente campos `vars`) al mismo tiempo.
  - Libera la lista clonada al final.

- `fill_tasks(t_render_task *tasks, t_scene **scene_clones, const t_app *app, const t_cam_frame *frame, int thread_count)`
  - Divide `height` en `thread_count` bloques de filas:
    - `rows_per_thread = height / thread_count`
    - `remainder = height % thread_count`
    - Los primeros `remainder` hilos reciben una fila extra (distribución balanceada).
  - Asigna:
    - `tasks[i].scene = scene_clones[i]` (copia privada).
    - `tasks[i].frame = frame` (solo lectura).
    - `tasks[i].framebuffer = app->framebuffer` (compartido, pero con filas exclusivas).
    - `tasks[i].width/height/show_normals/y_start/y_end`.

- `launch_workers(pthread_t *threads, t_render_task *tasks, int spawn_count)`
  - Lanza `spawn_count` hilos con `pthread_create`.
  - Si falla al crear algún hilo:
    - Espera los ya lanzados (`pthread_join`) y devuelve fallo (se hará fallback monohilo).

- `render_worker(void *param)`
  - Núcleo de cómputo por hilo:
    - Copia `*task->frame` en una variable local (`vars.frame`) para no puntear memoria compartida en cada acceso.
    - Itera `y ∈ [y_start, y_end)`, `x ∈ [0, width)`.
    - Calcula UV centrado en píxel (0.5 offset), rayo, traza y escribe color.
  - Llama a `trace_pixel(...)`.

- `trace_pixel(const t_scene *scene, t_ray r, int show_normals)`
  - Lógica de un píxel:
    - `scene_hit(scene, r, FLT_MAX, &hit)` (intersecciones).
    - Si `show_normals` y hay hit: visualiza la normal.
    - Si no: `shade_lambert(scene, &hit)`.

- `join_workers(pthread_t *threads, int spawn_count)`
  - Espera a todos los hilos creados.

- `render_scene_single_thread(t_app *app, const t_cam_frame *frame)`
  - Construye un `t_render_task` de toda la imagen y llama a `render_worker` en el hilo actual.

---

## División del trabajo (pantalla → hilos)

- Se divide la imagen por filas contiguas:
  - Pros:
    - Accesos lineales en memoria → buena localidad (cache-friendly).
    - Sin competencia de escritura: cada hilo escribe un bloque exclusivo del framebuffer.
  - Reparto equilibrado:
    - Las filas se distribuyen lo más uniformemente posible; si hay resto, los primeros hilos cogen una fila extra.

- Fórmulas:
  - `rows_per_thread = height / thread_count`
  - `remainder = height % thread_count`
  - Para el hilo i:
    - `row_count = rows_per_thread + (i < remainder)`
    - `y_start` acumulativo, `y_end = y_start + row_count`.

---

## Sincronización y seguridad de datos

- Deadlocks: no hay (no se usan mutex/condvars/barreras).
- Data races: se evitan por diseño:
  - Framebuffer compartido, pero particionado en bloques disjuntos de filas.
  - `t_cam_frame` es inmutable durante el render: cada worker toma una copia local.
  - Escena por hilo: `scene_clone` copia la lista de objetos. Así, cualquier escritura (p. ej., campos `vars` durante shading/intersección) no compite entre hilos.
  - MLX: todas las llamadas de UI/ventana ocurren en el hilo principal (no desde workers).

- Notas sobre recursos compartidos:
  - Texturas (bump) y otros punteros dentro de `u_obj` se asumen de solo lectura durante el render.
  - Si en el futuro algún material u objeto hiciera escrituras globales, debería moverse a datos por-hilo o protegerse explícitamente.

---

## Uso de pthreads (qué y por qué)

Funciones usadas:
- `pthread_create(&threads[i], NULL, render_worker, &tasks[i])`
  - Crea hilos POSIX portables (Linux).
  - Razonamiento: control total sobre función de trabajo, stack y vida del hilo.
- `pthread_join(threads[i], NULL)`
  - Espera la finalización ordenadamente.
  - Razonamiento: asegura que todas las escrituras al framebuffer han concluido antes de usarlo.

Funciones NO usadas:
- Mutex/condvars/semáforos: la arquitectura evita necesidad de sincronización (partición estática + copias por hilo).
- Barreras: la espera final se hace con joins; no hay sincronización intermedia.
- Pools persistentes: implementamos un “pool por frame” simple. Un pool fijo podría reutilizar hilos entre frames, pero aumentaría complejidad (cola de tareas, condvars). Se puede abordar como mejora futura.

---

## ¿Cómo se bloquea cada variable usada en un hilo?

No se “bloquea” con mutex; se evita el conflicto:

- `framebuffer`: cada hilo escribe una región exclusiva: índices [y_start, y_end) × [0, width).
- `t_scene`: cada hilo tiene un `scene_clone` distinto (lista de `t_object` independientes).
- `t_cam_frame`: se copia a variables locales del worker.
- `show_normals`, `width`, `height`: escalares inmutables dentro de cada `t_render_task`.

Con esta estrategia, no hay variables compartidas con escrituras concurrentes → no se necesitan locks.

---

## División del renderizado: cómo se logra

- Preparación (`fill_tasks`):
  - Se definen tareas “scanline-block” con límites exactos y datos necesarios para trazar.
- Ejecución:
  - `spawn_count` hilos con `pthread_create` + el hilo principal procesando el último bloque.
- Sincronización final:
  - `pthread_join` por cada hilo.

Esto es suficiente porque el render de cada píxel es independiente y el acceso al framebuffer está disjunto.

---

## Gestión de errores y fallbacks

- Si algo falla al reservar `threads/tasks` o al clonar escenas:
  - Se libera lo ya reservado y se cae a `render_scene_single_thread`.
- Si falla `pthread_create` para algún hilo:
  - Se hace `join` de los hilos ya lanzados y fallback a monohilo.

Así el programa no se cae por falta de recursos.

---

## Consideraciones de rendimiento y mejoras futuras

- Coste de clonar la escena:
  - Actual: clonado superficial por frame (lista de `t_object`). Es rápido, pero no gratis en escenas grandes.
  - Mejoras:
    - Reutilizar clones entre frames (pool de clones).
    - Un diseño “object-const” (no escribir en `vars` en render) permitiría compartir la escena sin clones.
- Balance de carga:
  - División por filas es sencilla y cache-friendly. Si el coste por fila no es uniforme, se puede usar “tiles” pequeños con cola de tareas (requiere mutex/condvar).
- Evitar falsas comparticiones:
  - Escribir por filas minimiza la contención en caché. En algunos sistemas, separar los bloques por hilos con padding podría reducir más el tráfico de caché (normalmente no necesario).
- Hilos persistentes (thread pool):
  - Permite reutilizar hilos entre frames evitando overhead de creación/union.
  - Requiere colas y sincronización → más complejidad.

---

## Depuración y verificación

- Artefactos de “ruido” o “bandas”:
  - Indican data races (p. ej., escritura concurrente de objetos o framebuffer).
  - Solución aplicada: `scene_clone` + filas exclusivas.
- Validar:
  - Comparar salida mono vs multi hilo (deberían ser idénticas en determinismo).
  - Usar `MINIRT_THREADS=1` para forzar monohilo y aislar problemas.
  - Probar con escenas con bump/checker para forzar rutas de shading diversas.

---

## Apéndice: Detección de número de hilos

Archivo: `src_bonus/minirt_bonus.c`

- `detect_thread_count`:
  - `MINIRT_THREADS` (entorno) para override manual.
  - `sysconf(_SC_NPROCESSORS_ONLN)` para detectar cores online.
  - Cap: 16 (coincide con `MAX_RENDER_THREADS`).
- `clamp_thread_count` en `render_bonus.c`:
  - Ajusta a [1, MAX] y no excede el número de filas.

---

## Resumen

- Paralelismo por filas con N hilos (N−1 workers + hilo principal).
- Sin locks ni condvars; seguridad por partición de datos y clones por hilo.
- `pthread_create`/`pthread_join` como únicas primitivas de sincronización.
- Fallback robusto a monohilo.
- Estructura simple, portable y eficaz para miniRT bonus.