# Tarea #6: Árboles y Grafos (CS2013 Programación III, 2026-2)

Implementación en C++17 de un BST iterativo, un Red-Black Tree y recorridos DFS/BFS iterativos. No se usa recursión en el BST, ni en DFS (Problema 3) ni en BFS (Problema 4). Solo se usan `std::stack` y `std::queue`.

## Estructura

```
include/bst.hpp          Ejercicio 1: BST (insert, remove, in-order, altura)
include/rbtree.hpp       Problema 2: Red-Black Tree (rotaciones, fixup, verify, altura)
include/binary_tree.hpp  Nodo binario genérico y constructor desde level-order
include/dfs.hpp          Problema 3: camino raíz-hoja con suma K (std::stack)
include/bfs.hpp          Problema 4: completo, ancho máximo, profundidad mínima (std::queue)
src/main.cpp             Pruebas y comparación experimental
```

## Compilar y ejecutar

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp -o tarea6 && ./tarea6
```

o con CMake:

```bash
cmake -S . -B build && cmake --build build && ./build/tarea6
```

El programa termina con código 0 si todas las verificaciones pasan. Probado con `-fsanitize=address,undefined` sin errores ni fugas.

Convención: la altura se mide en nodos (árbol vacío = 0, un solo nodo = 1).

## Resultados

Con 20 datos ascendentes (1..20):

| Estructura | Altura |
|---|---|
| BST | 20 |
| Red-Black Tree | 6 |

Tras eliminar 7 y 13 del BST, la altura es 18 (sigue siendo una cadena).

| n | BST | Red-Black | cota 2·log₂(n+1) |
|---|---|---|---|
| 20 | 20 | 6 | 8 |
| 100 | 100 | 11 | 13 |
| 1000 | 1000 | 17 | 19 |
| 10000 | 10000 | 24 | 26 |

## Ejercicio 1: por qué el BST se degrada a O(n)

Cada operación de un BST (búsqueda, inserción, eliminación) recorre un único camino desde la raíz, así que su costo es Θ(h), con h la altura. El BST solo impone el orden entre claves y no restringe la forma, por lo que h puede valer cualquier cosa entre ⌈log₂(n+1)⌉ y n.

**Afirmación.** Si se insertan k₁ < k₂ < … < kₙ en ese orden, el árbol resultante es una cadena derecha con kⱼ a profundidad j−1.

**Demostración por inducción en j.**
- Base: tras insertar k₁ el árbol es solo la raíz.
- Paso: supongamos que tras insertar k₁..kⱼ el árbol es la cadena k₁→k₂→…→kⱼ por hijos derechos. Como kⱼ₊₁ > kᵢ para todo i ≤ j, en cada nodo la comparación manda a la derecha. Al llegar a kⱼ, su hijo derecho es nulo, y kⱼ₊₁ se inserta ahí. La cadena queda extendida. ∎

Por tanto h = n y la inserción de kⱼ hace j−1 comparaciones. El costo total de insertar los n datos es

Σⱼ₌₁ⁿ (j−1) = n(n−1)/2 = Θ(n²),

y una búsqueda de kₙ cuesta n comparaciones = Θ(n). El caso peor de una operación es Θ(n), no O(log n). La cota O(log n) solo vale si h = O(log n), y el BST no garantiza eso: el único límite general es h ≤ n.

## Problema 2: por qué el Red-Black Tree tiene altura O(log n)

Sea bh(x) el número de nodos negros en un camino de x hasta una hoja nula, sin contar a x y contando la hoja nula (negra por la propiedad 5). Por la propiedad 4 es independiente del camino.

**Lema 1.** El subárbol con raíz x tiene al menos 2^bh(x) − 1 nodos internos.

Por inducción sobre la altura de x.
- Si x es hoja nula, bh(x) = 0 y 2⁰ − 1 = 0 nodos internos. Se cumple.
- Si x es interno, cada hijo tiene bh(x) (si es rojo) o bh(x) − 1 (si es negro), es decir, bh ≥ bh(x) − 1. Por hipótesis inductiva cada hijo tiene al menos 2^(bh(x)−1) − 1 nodos internos. Entonces el subárbol de x tiene al menos 2·(2^(bh(x)−1) − 1) + 1 = 2^bh(x) − 1. ∎

**Lema 2.** Si h es la altura de la raíz (en nodos), bh(raíz) ≥ h/2.

Por la propiedad 3 no hay dos nodos rojos consecutivos, así que en el camino más largo (h nodos sin contar la raíz, incluyendo la hoja nula negra) al menos la mitad son negros. ∎

**Teorema.** Un Red-Black Tree con n nodos tiene altura h ≤ 2·log₂(n+1).

Por los lemas, n ≥ 2^bh − 1 ≥ 2^(h/2) − 1. Despejando: n + 1 ≥ 2^(h/2), es decir, h ≤ 2·log₂(n+1) = O(log n). ∎

Esto se verifica en `main.cpp` para cada n probado (ver tabla de resultados).

**Por qué se mantiene tras insertar.** El nodo nuevo entra rojo, lo que preserva la propiedad 4. Solo puede violarse la 3 (padre rojo) o la 2 (raíz roja). `fixInsert` actúa según el color del tío:
- Tío rojo: se recolorean padre, tío y abuelo, y el problema sube dos niveles. Termina a lo sumo en la raíz, que se recolorea de negro.
- Tío negro: una o dos rotaciones (caso zig-zag, luego zig-zig) corrigen la violación en O(1) y terminan el arreglo.

La inserción cuesta O(log n) (descenso más a lo sumo O(log n) recoloraciones y 2 rotaciones).

## Problema 3: DFS iterativo

Se busca un camino raíz-hoja con suma K. Con `std::stack<Frame>`, donde cada `Frame` guarda `{nodo, suma del prefijo, estado}` (estado 0: recién visitado, 1: ya se exploró el hijo izquierdo, 2: ya se exploró el derecho).

Invariante: en todo momento el contenido del stack es exactamente el camino de la raíz al nodo actual, y `sum` es la suma de ese prefijo. Al llegar a una hoja con `sum == K`, se vacía el stack para obtener el camino (en orden inverso) y se invierte.

**Por qué DFS es adecuado.** La propiedad buscada depende de un camino completo raíz→hoja, y DFS recorre justamente un camino a la vez, con backtracking natural: al retroceder se descarta el nodo del tope y la suma del prefijo ya está guardada en el frame anterior. La memoria es O(h) porque solo está el camino actual. Con BFS habría que mantener a la vez la suma parcial y el camino de cada nodo de la frontera, que puede tener O(n) nodos. No se poda por suma porque los valores pueden ser negativos.

Complejidad: O(n) tiempo (cada nodo entra y sale una vez), O(h) espacio.

## Problema 4: BFS iterativo

Con `std::queue`. Tres métricas:
- **Completo:** se encolan también los hijos nulos. El árbol es completo si y solo si, en el recorrido por niveles, después del primer nulo no aparece ningún nodo real (los nodos ocupan posiciones contiguas 0..n−1 del arreglo implícito, donde los hijos de la posición i están en 2i+1 y 2i+2).
- **Ancho máximo:** máximo del tamaño de la cola al comenzar cada nivel (cantidad de nodos reales por nivel).
- **Profundidad mínima:** nivel de la primera hoja que se desencola.

**Justificación formal.** Invariante de la cola: en todo momento contiene nodos de profundidad d seguidos de nodos de profundidad d+1 (y nada más). Por inducción: inicialmente solo está la raíz (d = 0); al desencolar un nodo de profundidad d se encolan sus hijos, de profundidad d+1, al final, lo que mantiene la forma. Por tanto BFS desencola los nodos en orden no decreciente de profundidad. Consecuencias:
1. Los nodos de cada nivel salen de la cola de forma contigua, así que el tamaño de la cola al inicio de un nivel es el ancho de ese nivel.
2. La primera hoja desencolada tiene profundidad mínima: si existiera una hoja más superficial, habría salido antes.
3. El orden por niveles coincide con el orden de posiciones del arreglo implícito, lo que permite detectar huecos para la completitud.

DFS no sirve para estas métricas sin información extra, porque visita en orden de profundidad y no de nivel.

Complejidad: O(n) tiempo, O(w) espacio con w el ancho máximo.
