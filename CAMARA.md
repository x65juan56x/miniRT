## Cámara pinhole en miniRT:
### Descomposición de los cálculos clave

**Entradas**

- pos: posición de la cámara (origen de los rayos).
- dir: dirección de mirada (unitaria).
- FOV: campo de visión horizontal en grados.
- width, height: resolución de la ventana.
- focal: distancia del plano de imagen al ojo; se fija a 1.0 por simplicidad.

**1. Base ortonormal de cámara (forward, right, up)**
- forward = normalize(dir)
	- Propósito: eje Z de la cámara (a dónde miras).
- up_world: vector de “arriba” global, típicamente (0,1,0). Si |dot(forward, up_world)| > 0.999, usa otro (0,0,1) para evitar degeneración.
- right = normalize(cross(forward, up_world))
	- Propósito: eje X de la cámara; garantiza perpendicularidad a forward.
- up = cross(right, forward)
	- Propósito: eje Y de la cámara; queda automáticamente ortonormal al resto.

	**Por qué?:** esta base permite convertir coordenadas de cámara (u,v) a mundo, respetando orientación de la cámara incluso si está inclinada o girada.

**2. De FOV a tamaño del plano**
- hfov_rad = deg2rad(FOV)
- half_width = tan(hfov_rad/2) · focal
	- Propósito: fija el ancho “físico” del plano a partir del FOV.
- aspect = width / height
- half_height = half_width / aspect
	- Propósito: mantener la relación de aspecto sin distorsiones.

	**Ventaja:** cambiar resolución no altera el FOV ni deforma la imagen; solo cambia la densidad de píxeles.

**3. Anclajes del plano de imagen**
- origin = pos.
- center = origin + forward · focal
	- Propósito: centro del plano a distancia focal delante del ojo.
- horizontal = right · (2 · half_width)
	- Propósito: vector que cubre el ancho completo del plano.
- vertical = up · (2 · half_height)
	- Propósito: vector que cubre el alto completo del plano.

**Nota sobre coordenadas de ventana:** En MLX, Y de pantalla crece hacia abajo:

- Opción 1: invertir v más adelante: v = 1 − v
- Opción 2: invertir vertical aquí: vertical = −up · (2 · half_height)

**4. Esquina inferior izquierda del plano**
- lower_left = center − 0.5 · horizontal − 0.5 · vertical
	- Propósito: punto de referencia desde el cual barrer el plano con u y v en [0,1].

**5. Mapeo píxel → coordenadas UV**
- u = (x + 0.5) / width
- v = (y + 0.5) / height
	- Propósito: muestrear en el centro de cada píxel para reducir aliasing geométrico básico. Si usas convención de pantalla con Y hacia abajo, invierte v: v = 1 − v (o usa vertical invertido como arriba).

**6. Punto en el plano e inicialización del rayo**
- P = lower_left + u · horizontal + v · vertical.
	- Propósito: punto del plano de imagen correspondiente al píxel.
- dir_rayo = normalize(P − origin).
	- Propósito: dirección del rayo primario para ese píxel.

**7. Rango y validaciones**
- FOV debe estar en (0, 180). Valores extremos producen tan(FOV/2) muy grandes o indefinidos.
- dir debe ser unitario. Si no, normalízalo para evitar escalas erróneas en el plano.
- Maneja el caso casi paralelo a up_world con el fallback para up_world.

**8. Aspectos numéricos y de diseño**
- Fijar focal = 1.0 simplifica; cambiar focal equivale a cambiar FOV efectivo (half_width = tan(FOV/2) · focal).
- Usar half_width y half_height desacopla cámara de la resolución; cambiar width/height no altera composición.
- Handedness: right = normalize(cross(forward, up_world)) y up = cross(right, forward) mantiene un sistema coherente. Si inviertes el orden del cross, inviertes la mano del sistema.

**9. Alternativas y extensiones**
- Para invertir la Y sin tocar v, define vertical con signo negativo.
- Depth of Field (bonus): mantiene este marco; se añaden apertura y plano focal, pero el plano de imagen y los rayos base se construyen igual.
- Anti-aliasing (bonus): muestrea varias (u,v) por píxel con jitter y promedia.

**Resumen conceptual**
- La cámara es el origen; el plano de imagen está delante a distancia fija (focal).
- El FOV horizontal y el aspect dictan el tamaño del plano (no depende de la resolución).
- La base ortonormal convierte el plano ideal de cámara en coordenadas del mundo.
- Cada píxel produce un rayo pasando por su punto en el plano y el origen de cámara.

---
---
 ## Idea de estructura para los datos del frame

```h
typedef struct s_cam_frame
{
	t_vec3	origin;
	t_vec3	forward;
	t_vec3	right;
	t_vec3	up;
	t_vec3	horizontal;
	t_vec3	vertical;
	t_vec3	lower_left;
}	t_cam_frame;

void	camera_build_frame(const t_camera *cam, int width, int height,
			t_cam_frame *out);
```

```c
void	camera_build_frame(const t_camera *cam, int width, int height,
			t_cam_frame *out)
{
	t_vec3	up_world;
	float	aspect;
	float	half_w;
	float	half_h;
	float	focal;
	t_vec3 center;

	out->origin = cam->pos;
	out->forward = v3_norm(cam->dir);
	up_world = v3(0.0f, 1.0f, 0.0f);
	if (fabsf(v3_dot(out->forward, up_world)) > 0.999f)
		up_world = v3(0.0f, 0.0f, 1.0f);
	out->right = v3_norm(v3_cross(out->forward, up_world));
	out->up = v3_cross(out->right, out->forward);
	focal = 1.0f;
	aspect = (float)width / (float)height;
	half_w = tanf(deg2rad(cam->fov_deg) * 0.5f) * focal;
	half_h = half_w / aspect;
	out->horizontal = v3_mul(out->right, 2.0f * half_w);
	out->vertical = v3_mul(out->up, 2.0f * half_h);
	center = v3_add(out->origin, v3_mul(out->forward, focal));
	out->lower_left = v3_sub(v3_sub(center, v3_mul(out->horizontal, 0.5f)), v3_mul(out->vertical, 0.5f));
}
```

## EJEMPLOS

### Ejemplo 1: Esfera roja centrada

- width: 800
- height: 600

**s_camera**
- pos:      (0.0, 0.0, 0.0)
- dir:      (0.0, 0.0, -1.0)
- fov_deg:  90.0

**Plano de imagen:**
- focal: 1.0
- aspect: 800/600 = 1.333
- hfov_rad: π/2 ≈ 1.5708
- half_width: tan(1.5708/2) * 1.0 ≈ 1.0
- half_height: 1.0 / 1.333 ≈ 0.75
- center: (0, 0, -1)

**s_cam_frame**
- origin:      (0.0, 0.0, 0.0)
- forward:     (0.0, 0.0, -1.0)
- right:       (1.0, 0.0, 0.0)
- up:          (0.0, 1.0, 0.0)
- horizontal:  (2.0, 0.0, 0.0)
- vertical:    (0.0, 1.5, 0.0)
- lower_left:  (-1.0, -0.75, -1.0)

**s_sphere**
- center:  (0.0, 0.0, -3.0)
- di:      1.0
- color:   (1.0, 0.0, 0.0)

**Objeto:**
- Esfera: centro (0, 0, -3), radio 0.5, color (1, 0, 0)

**Visualización:**
- La esfera roja aparece centrada en la pantalla, a 3 unidades delante de la cámara.

---

### Ejemplo 2: Plano horizontal (suelo) y esfera azul

- width: 800
- height: 800

**s_camera**
- pos:      (0.0, 1.0, 2.0)
- dir:      (0.0, -0.287, -0.957)  // normalizado de (0, -0.3, -1)
- fov_deg:  60.0

**Plano de imagen:**
- focal: 1.0
- aspect: 1.0
- hfov_rad: π/3 ≈ 1.047
- half_width: tan(1.047/2) * 1.0 ≈ 0.577
- half_height: 0.577
- center: (0, 1, 2) + forward * 1.0

**s_cam_frame**
- origin:      (0.0, 1.0, 2.0)
- forward:     (0.0, -0.287, -0.957)
- right:       (1.0, 0.0, 0.0)
- up:          (0.0, 0.957, -0.287)
- horizontal:  (1.154, 0.0, 0.0)
- vertical:    (0.0, 1.154, 0.0)
- lower_left:  (-0.577, 0.423, 1.043)

**s_sphere**
- center:  (0.0, 0.5, -2.0)
- di:      1.0
- color:   (0.0, 0.0, 1.0)

**s_plane**
- point:   (0.0, 0.0, 0.0)
- normal:  (0.0, 1.0, 0.0)
- color:   (0.8, 0.8, 0.8)

**Objetos:**
- Plano: punto (0, 0, 0), normal (0, 1, 0), color (0.8, 0.8, 0.8)
- Esfera: centro (0, 0.5, -2), radio 0.5, color (0, 0, 1)

**Visualización:**
- El plano es el suelo, la esfera azul está sobre el suelo, vista ligeramente desde arriba.

---

### Ejemplo 3: Dos esferas, cámara desplazada

- width: 600
- height: 400

**s_camera**
- pos:      (2.0, 1.0, 2.0)
- dir:      (-0.669, -0.201, -0.715) // normalizado de (-1, -0.3, -1)
- fov_deg:  45.0

**Plano de imagen:**
- focal: 1.0
- aspect: 1.0
- hfov_rad: π/3 ≈ 1.047
- half_width: tan(1.047/2) * 1.0 ≈ 0.577
- half_height: 0.577
- center: (0, 1, 2) + forward * 1.0

**s_cam_frame**
- origin:      (2.0, 1.0, 2.0)
- forward:     (-0.669, -0.201, -0.715)
- right:       (0.715, 0.0, -0.699)
- up:          (-0.140, 0.980, -0.140)
- horizontal:  (0.592, 0.0, -0.578)
- vertical:    (0.0, 0.552, 0.0)
- lower_left:  (1.704, 0.724, 2.289)

**s_sphere**
- center:  (0.0, 0.5, -1.0)
- di:      1.0
- color:   (1.0, 0.0, 0.0)

- center:  (1.0, 0.5, -2.0)
- di:      1.0
- color:   (0.0, 1.0, 0.0)

**Objetos:**
- Esfera 1: centro (0, 0.5, -1), radio 0.5, color (1, 0, 0)
- Esfera 2: centro (1, 0.5, -2), radio 0.5, color (0, 1, 0)

**Visualización:**
- La cámara está desplazada a la derecha y arriba, mirando hacia las dos esferas, que aparecen en perspectiva.

---

### Ejemplo 4: Cilindro vertical y plano de fondo

- width: 1024
- height: 768

**s_camera**
- pos:      (0.0, 1.0, 5.0)
- dir:      (0.196, -0.039, -0.980) // normalizado de (0, -0.2, -1)
- fov_deg:  70.0

**Plano de imagen:**
- focal: 1.0
- aspect: 1024/768 ≈ 1.333
- hfov_rad: 70° = 1.221 rad
- half_width: tan(1.221/2) * 1.0 ≈ 0.672
- half_height: 0.672 / 1.333 ≈ 0.504
- center: (0, 1, 5) + forward * 1.0

**s_cam_frame**
- origin:      (0.0, 1.0, 5.0)
- forward:     (0.196, -0.039, -0.980)
- right:       (0.980, 0.0, 0.196)
- up:          (0.008, 0.999, -0.020)
- horizontal:  (1.344, 0.0, 0.264)
- vertical:    (0.0, 1.008, 0.0)
- lower_left:  (-0.672, 0.496, 4.868)

**s_plane**
- point:   (0.0, 0.0, -2.0)
- normal:  (0.0, 0.0, 1.0)
- color:   (0.7, 0.7, 0.7)

**s_cyl**
- center:  (0.0, 0.0, 0.0)
- axis:    (0.0, 1.0, 0.0)
- di:      0.5
- he:      2.0
- color:   (1.0, 1.0, 0.0)

**Objetos**
- Cilindro: centro (0, 0, 0), eje (0, 1, 0), diámetro 0.5, altura 2.0, color (1, 1, 0)
- Plano: punto (0, 0, -2), normal (0, 0, 1), color (0.7, 0.7, 0.7)

**Visualización:**
- El cilindro amarillo está de pie en el centro, el plano gris es el fondo detrás.

---

### Ejemplo 5: FOV extremo (gran angular)

- width: 800
- height: 600

**s_camera**
- pos:      (0.0, 0.0, 0.0)
- dir:      (0.0, 0.0, -1.0)
- fov_deg:  120.0

**Plano de imagen:**
- focal: 1.0
- aspect: 1.333
- hfov_rad: 2.094 rad
- half_width: tan(2.094/2) * 1.0 ≈ 1.732
- half_height: 1.732 / 1.333 ≈ 1.299
- center: (0, 0, -1)

**s_cam_frame**
- origin:      (0.0, 0.0, 0.0)
- forward:     (0.0, 0.0, -1.0)
- right:       (1.0, 0.0, 0.0)
- up:          (0.0, 1.0, 0.0)
- horizontal:  (3.464, 0.0, 0.0)
- vertical:    (0.0, 2.598, 0.0)
- lower_left:  (-1.732, -1.299, -1.0)

**s_sphere**
- center:  (0.0, 0.0, -3.0)
- di:      1.0
- color:   (0.0, 1.0, 1.0)

**Objetos**
- Esfera: centro (0, 0, -3), radio 0.5, color (0, 1, 1)

**Visualización:**
- La esfera aparece pequeña y el fondo se ve muy “abierto”, con distorsión de gran angular.
