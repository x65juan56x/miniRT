Si yo estoy declarando una variable como "float", que sentido tiene asignarle un valor como "1.0f" en lugar de "1", cual es la diferencia??

La diferencia está en el **tipo de dato** y en la **precisión** con la que el compilador trata el valor:

- `1` es un **entero** (`int`).
- `1.0` es un **double** (coma flotante de doble precisión).
- `1.0f` es un **float** (coma flotante de simple precisión).

Cuando declaras una variable como `float`, es mejor asignarle valores con el sufijo `f` (por ejemplo, `1.0f`) porque:

1. **Evitas conversiones implícitas:**  
Si escribes `float x = 1.0;`, el compilador crea un `double` y luego lo convierte a `float`, lo que puede ser menos eficiente y puede generar warnings.

2. **Claridad de tipo:**  
`1.0f` deja claro que quieres un `float`, no un `double` ni un `int`.

3. **Precisión y rendimiento:**  
Usar `1.0f` asegura que todas las operaciones se hagan en simple precisión, que es más rápido en muchas arquitecturas y evita conversiones innecesarias.

**Resumen:**  
Siempre que trabajes con variables `float`, usa literales con sufijo `f` (como `1.0f`) para evitar conversiones y dejar claro el tipo de dato.

---
---

alias valdy="valgrind --leak-check=full --show-leak-kinds=all -s --track-origins=yes" #--suppressions=mlx.supp"

valdy ./miniRT_bonus examples/scenes/PRUEBAS.rt &> valgrind.log