## Sombras duras (rayos de sombra)

- Si nada tapa la luz: color = ambiente + difuso (Lambert).
- Si algo tapa la luz: color = solo ambiente (opcionalmente modulado por el albedo del material del punto sombreado).

### Modelo mental: rayo de sombra

Para cada punto de impacto $P$ que hemos sombreado con Lambert, lanzamos un segundo rayo (rayo de sombra) hacia la posición de la luz $L$:

1. Calculamos $\vec{toL} = L - P$ y su longitud $d = \|\vec{toL}\|$.
2. La dirección normalizada es $\hat{L} = \vec{toL} / d$.
3. Definimos un rayo de sombra $r_s$ con origen ligeramente desplazado desde $P$ en dirección a la luz para evitar auto-intersecciones: $P_\epsilon = P + \epsilon \cdot \hat{L}$.
4. Trazamos $r_s = (P_\epsilon, \hat{L})$ y comprobamos si intersecta con algún objeto a una distancia $t$ tal que $0 < t < d$.
   - Si existe tal intersección, el punto está en sombra (la luz está ocluida).
   - Si no existe, el punto está iluminado directamente.

Notas clave:
- $\epsilon$ (EPSILON) es un sesgo numérico pequeño (p. ej. $10^{-4}$) para evitar el llamado “shadow acne” (auto-sombra por errores de coma flotante).
- Limitamos el alcance del rayo a $d$ (distancia a la luz) para ignorar intersecciones “más allá” de la luz.

### Fórmula de color con sombras

En notación vectorial (componentes R, G, B en [0,1]):

- Si NO hay oclusión:

$$
C = C_\text{amb} + \big(A \odot C_\text{luz} \cdot \text{bright}\big) \cdot \max(0, \langle N, \hat{L} \rangle)
$$

- Si HAY oclusión (sombra):

Ambiente modulado por albedo:

$$
C = A \odot C_\text{amb}
$$

Multiplicamos el ambiente por el albedo del material en sombra. Esto mantiene el “color propio” del objeto también en las zonas en sombra.

### Pasos concretos en el código

1. Intersección principal (`intersect_scene`)

2. Test de sombra (`in_shadow`)
   - Calcula `toL = L.pos - P`, `maxD = ||toL||`.
   - Si `maxD <= 0`: no hay sombra.
   - `dir = toL / maxD`.
   - Construye un rayo de sombra desde `P_eps = P + EPSILON * dir` con dirección `dir`.
   - Recorre los objetos; para cada uno calcula `t` de intersección con `r_s`.
   - Si existe un `t` con `0 < t < maxD`: devolver `true` (hay oclusión).

3. Sombreado con decisión de sombra (`shade_lambert_shadow`)
   - Si no hubo hit: color negro.
   - Si `in_shadow(...)` es verdadero: devolver `albedo ⊙ ambient`.
   - Si no: calcular Lambert y sumar ambiente.

### Notas

- Shadow acne (auto-sombra):
  - Usa un `EPSILON` pequeño. Demasiado pequeño → aún hay acne; demasiado grande → sombras separadas (“Peter Panning”).
  - Alternativa: desplazar el origen con la normal: `P + EPSILON * N` (reduce fugas cuando la luz está casi rasante). Desplazamos en dirección a la luz por simplicidad.

- Límite de distancia:
  - Ignora hits con `t >= maxD` (objetos detrás de la luz no deben oscurecer).

- Planos y caras traseras:
  - `hit_plane` no tiene que devolver intersecciones con `t <= 0`.
  - Los planos infinitos, pueden producir sombras “infinitas”; es el comportamiento esperado.

- Dentro de un objeto:
  - Si `P` está dentro de una esfera, casi siempre habrá oclusión. Eso es físicamente correcto para materiales opacos.

- Performance:
  - El rayo de sombra añade una intersección por píxel. Para muchas luces/objetos, considera estructuras de aceleración (BVH) en el futuro.

### Resumen

- Tenemos $P$, $N$ y `albedo` del primer hit.
- Calculamos `toL`, `maxD` y `dir = toL / maxD`.
- Lanzamos rayo de sombra desde `P + EPSILON * dir` y compruebamos si hay hit con `0 < t < maxD`.
- Si hay oclusión → color = `albedo ⊙ ambient`; si no, color = `ambient + albedo ⊙ light * bright * max(0, N·L̂)`.
- Conviertimos a RGBA y pintamos el píxel.
