# main10 – Triángulos (intersección, sombreado y uso)

## Datos y primitivas involucradas

- Triángulo: tres vértices `a`, `b`, `c` (cada uno `t_vec3`).
- Estructuras existentes: rayos (`t_ray`), escena (`t_scene`), materiales simples (albedo directo), cámara, luz, etc.
- En el parser, el triángulo se declara con la línea `tr` en el `.rt`.

## Intersección rayo–triángulo (Möller–Trumbore)

Se usa el algoritmo de Möller–Trumbore sin culling: acepta tanto triángulos vistos por delante como por detrás (determinante con signo). Pasos (ver función `hit_triangle`):

1) Construcción de aristas del triángulo:
- `e1 = b - a`
- `e2 = c - a`

2) Productos auxiliares:
- `pvec = cross(r.dir, e2)`
- `det = dot(e1, pvec)`
- Si `|det| < 1e-8` → rayo paralelo al triángulo → no hay intersección.
- `inv_det = 1 / det`

3) Barycentría – coordenada `u`:
- `tvec = r.orig - a`
- `u = dot(tvec, pvec) * inv_det`
- Si `u < 0` o `u > 1` → fuera del triángulo.

4) Barycentría – coordenada `v`:
- `qvec = cross(tvec, e1)`
- `v = dot(r.dir, qvec) * inv_det`
- Si `v < 0` o `u + v > 1` → fuera del triángulo.

5) Distancia sobre el rayo:
- `t = dot(e2, qvec) * inv_det`
- Si `t <= 0` → intersección detrás del origen del rayo (descartada).

6) Éxito:
- Devolver `t` como distancia de impacto. El punto es `P = r.orig + t * r.dir`.

Notas:
- Sin culling: el signo de `det` permite golpear ambas caras del triángulo.
- Tolerancias: se usa `1e-8` para evitar divisiones numéricamente inestables.

### ¿Por qué Möller–Trumbore?

- Resuelve en una sola pasada las incógnitas de intersección: la distancia sobre el rayo $t$ y las coordenadas baricéntricas $(u,v)$.
- Evita resolver un sistema 3×3 general con matrices; solo usa productos cruz/dot y pocas operaciones escalares.
- Es estable y muy usado en ray tracers por su buena relación coste/robustez.

### Formulación matemática (derivación breve)

Un rayo se define como:

$$ r(t) = O + t\,D, \quad t \in \mathbb{R} $$

Un punto del triángulo $(A,B,C)$ puede escribirse en coordenadas baricéntricas como:

$$ P = A + u\,e_1 + v\,e_2, \quad e_1 = B - A, \; e_2 = C - A $$

con las restricciones $u \ge 0,\; v \ge 0,\; u + v \le 1$ (interior del triángulo). En la intersección queremos $r(t) = P$:

$$ O + t\,D = A + u\,e_1 + v\,e_2 $$

Möller–Trumbore elimina $t$ aplicando productos cruzados estratégicos. Definimos:

$$ p = D \times e_2, \quad \det = e_1 \cdot p $$

Si $|\det|$ es muy pequeño, el rayo es casi paralelo al plano del triángulo y no hay intersección fiable. Si no, $\operatorname{inv\_det} = 1/\det$ y con $t_{vec} = O - A$:

$$ u = (t_{vec} \cdot p)\,\operatorname{inv\_det} $$
$$ q = t_{vec} \times e_1 $$
$$ v = (D \cdot q)\,\operatorname{inv\_det} $$
$$ t = (e_2 \cdot q)\,\operatorname{inv\_det} $$

Condiciones de aceptación:

- $u \in [0,1]$, $v \in [0,1]$, $u+v \le 1$ (interior del triángulo)
- $t > 0$ (delante del origen del rayo)

Complejidad constante por triángulo: unos pocos `cross` y `dot` por prueba.

## Normal del triángulo y orientación

- Normal geométrica plana: `n = norm(cross(e1, e2))`.
- Orientación frente al rayo: si `dot(n, r.dir) > 0`, se invierte `n` para que apunte “hacia la cámara”. Esto reduce artefactos de iluminación cuando golpeamos la cara posterior.

Actualmente no hay interpolación por normales de vértice (Gouraud/Phong). El triángulo es lambertiano plano con un único albedo.

Notas:
- La normal geométrica del triángulo corresponde al vector perpendicular al plano definido por $(A,B,C)$. Para coherencia visual, si $\mathbf{n}\cdot D>0$ (golpe por la cara posterior), se invierte para iluminar con la misma convención que el resto de primitivas.

### ¿Qué es la barycentría? ¿Qué significan u y v?

Las coordenadas baricéntricas $(u,v,w)$ satisfacen $u+v+w=1$ con $u,v,w\ge 0$. En Möller–Trumbore obtenemos $u$ y $v$ directamente; $w$ se recupera como $w=1-u-v$.

- Geométricamente, $(u,v,w)$ son pesos que combinan los vértices: $$P = u\,B + v\,C + w\,A.$$  Cuando todos están en $[0,1]$ y suman 1, $P$ cae dentro del triángulo.
- En los bordes: por ejemplo, $u=0$ significa el borde opuesto a $B$ (segmento $AC$). En los vértices: $(u,v,w)=(1,0,0)$ da $B$, $(0,1,0)$ da $C$, $(0,0,1)$ da $A$.
- Interpretación de áreas: cada peso es proporcional al área del subtriángulo opuesto al vértice correspondiente.

## Formato .rt para triángulos

Triángulo (extensión de la práctica):

```
tr ax,ay,az bx,by,bz cx,cy,cz R,G,B
```

- `a`, `b`, `c`: vértices del triángulo.
- `R,G,B`: albedo en 0..255 por canal (se normaliza a [0,1]).

Ejemplo mínimo (dos esferas + un triángulo):

```
A 0.10 255,255,255
C 0,1,5  0,0,-1  60
L 5,5,5  0.8  255,220,200

sp  0,0,0   2.0   200,20,20
sp  3,0,-1  1.0   20,200,20

tr -1,0,-2  1,0,-2  0,1,-2  200,200,200
```

### ¿Culling o sin culling?

- Sin culling (lo que usamos):
	- Pros: Robusto ante modelos con winding inconsistente o superficies bidireccionales (planos infinitos, láminas). Evita “agujeros” por caras traseras mal orientadas en el modelo.
	- Contras: Se testean también caras traseras; ligero coste extra si la malla está cerrada y bien orientada.

- Con culling (por ejemplo, aceptar solo $\det>\varepsilon$):
	- Pros: En mallas cerradas con normales consistentes suele ahorrar ≈50% de test en triángulos, mejorando rendimiento.
	- Contras: Puede perder intersecciones válidas si el winding es incorrecto o si esperas superficies visibles por ambas caras.

Recomendación práctica:
- En un miniRT educativo y con escenas mixtas, “sin culling” es más tolerante y predecible. Si más adelante importas mallas grandes bien orientadas, puedes activar culling como opción para ganar rendimiento.

### Consideraciones numéricas y casos borde

- Paralelismo: cuando $|\det| \approx 0$, rechazamos por estabilidad.
- Degenerados: triángulos con área casi nula (vértices colineales) no deben intersectar de forma fiable.
- Bordes/vértices: por redondeo, $u$ o $v$ pueden quedar en $(-\epsilon,1+\epsilon)$. Se suele permitir una tolerancia pequeña o clamp para decidir “dentro”.
- Self-intersection: al lanzar rayos secundarios (sombras), se desplaza el origen con un $\varepsilon$ a lo largo de la dirección para evitar re-impactar la misma superficie.
