

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
 
| $n$ | BST | Red-Black | cota $\lfloor 2\log_2(n+1) \rfloor$ |
|---|---|---|---|
| 20 | 20 | 6 | 8 |
| 100 | 100 | 11 | 13 |
| 1000 | 1000 | 17 | 19 |
| 10000 | 10000 | 24 | 26 |
 
## Ejercicio 1: por qué el BST se degrada a $O(n)$
 
Cada operación de un BST (búsqueda, inserción, eliminación) recorre un único camino desde la raíz, así que su costo es $\Theta(h)$, con $h$ la altura. El BST solo impone el orden entre claves y no restringe la forma, por lo que $h$ puede valer cualquier cosa entre $\lceil \log_2(n+1) \rceil$ y $n$.
 
**Afirmación.** Si se insertan $k_1 < k_2 < \dots < k_n$ en ese orden, el árbol resultante es una cadena derecha con $k_j$ a profundidad $j-1$.
 
**Demostración por inducción en $j$.**
 
- Base: tras insertar $k_1$ el árbol es solo la raíz.
- Paso: supongamos que tras insertar $k_1, \dots, k_j$ el árbol es la cadena $k_1 \to k_2 \to \dots \to k_j$ por hijos derechos. Como $k_{j+1} > k_i$ para todo $i \le j$, en cada nodo la comparación manda a la derecha. Al llegar a $k_j$, su hijo derecho es nulo y $k_{j+1}$ se inserta ahí. La cadena queda extendida. $\blacksquare$
Por tanto $h = n$ y la inserción de $k_j$ hace $j-1$ comparaciones. El costo total de insertar los $n$ datos es
 
$$\sum_{j=1}^{n} (j-1) = \frac{n(n-1)}{2} = \Theta(n^2)$$
 
y una búsqueda de $k_n$ cuesta $n$ comparaciones, es decir $\Theta(n)$. El caso peor de una operación es $\Theta(n)$, no $O(\log n)$. La cota $O(\log n)$ solo vale si $h = O(\log n)$, y el BST no garantiza eso: el único límite general es $h \le n$.
 
## Problema 2: por qué el Red-Black Tree tiene altura $O(\log n)$
 
Sea $bh(x)$ el número de nodos negros en un camino desde $x$ hasta una hoja nula, sin contar a $x$ y contando la hoja nula (negra por la propiedad 5). Por la propiedad 4 no depende del camino elegido.
 
**Lema 1.** El subárbol con raíz $x$ tiene al menos $2^{bh(x)} - 1$ nodos internos.
 
Por inducción sobre la altura de $x$.
 
- Si $x$ es hoja nula, $bh(x) = 0$ y $2^0 - 1 = 0$ nodos internos. Se cumple.
- Si $x$ es interno, cada hijo tiene $bh = bh(x)$ (si es rojo) o $bh = bh(x) - 1$ (si es negro), es decir, $bh \ge bh(x) - 1$. Por hipótesis inductiva cada hijo tiene al menos $2^{bh(x)-1} - 1$ nodos internos. Entonces el subárbol de $x$ tiene al menos
$$2\left(2^{bh(x)-1} - 1\right) + 1 = 2^{bh(x)} - 1 \quad \blacksquare$$
 
**Lema 2.** Si $h$ es la altura de la raíz (en nodos), entonces $bh(\text{raíz}) \ge h/2$.
 
Por la propiedad 3 no hay dos nodos rojos consecutivos, así que en el camino más largo ($h$ nodos sin contar la raíz, incluyendo la hoja nula negra) al menos la mitad son negros. $\blacksquare$
 
**Teorema.** Un Red-Black Tree con $n$ nodos tiene altura $h \le 2\log_2(n+1)$.
 
Por los lemas:
 
$$n \ge 2^{bh} - 1 \ge 2^{h/2} - 1 \;\Longrightarrow\; n + 1 \ge 2^{h/2} \;\Longrightarrow\; h \le 2\log_2(n+1) = O(\log n) \quad \blacksquare$$
 
Esto se verifica en `main.cpp` para cada $n$ probado (ver tabla de resultados).
 
**Por qué se mantiene tras insertar.** El nodo nuevo entra rojo, lo que preserva la propiedad 4. Solo puede violarse la 3 (padre rojo) o la 2 (raíz roja). `fixInsert` actúa según el color del tío:
 
- Tío rojo: se recolorean padre, tío y abuelo, y el problema sube dos niveles. Termina a lo sumo en la raíz, que se recolorea de negro.
- Tío negro: una o dos rotaciones (caso zig-zag, luego zig-zig) corrigen la violación en $O(1)$ y terminan el arreglo.
La inserción cuesta $O(\log n)$: el descenso, más a lo sumo $O(\log n)$ recoloraciones y 2 rotaciones.
 
## Problema 3: DFS iterativo
 
Se busca un camino raíz-hoja con suma $K$. Se usa `std::stack<Frame>`, donde cada `Frame` guarda `{nodo, suma del prefijo, estado}` (estado 0: recién visitado, 1: ya se exploró el hijo izquierdo, 2: ya se exploró el derecho).
 
Invariante: en todo momento el contenido del stack es exactamente el camino de la raíz al nodo actual, y `sum` es la suma de ese prefijo. Al llegar a una hoja con `sum == K`, se vacía el stack para obtener el camino (en orden inverso) y se invierte.
 
**Por qué DFS es adecuado.** La propiedad buscada depende de un camino completo raíz→hoja, y DFS recorre justamente un camino a la vez, con backtracking natural: al retroceder se descarta el nodo del tope y la suma del prefijo ya está guardada en el frame anterior. La memoria es $O(h)$ porque solo está el camino actual. Con BFS habría que mantener a la vez la suma parcial y el camino de cada nodo de la frontera, que puede tener $O(n)$ nodos. No se poda por suma porque los valores pueden ser negativos.
 
Complejidad: $O(n)$ en tiempo (cada nodo entra y sale una vez) y $O(h)$ en espacio.
 
## Problema 4: BFS iterativo
 
Se usa `std::queue`. Tres métricas:
 
- **Completo:** se encolan también los hijos nulos. El árbol es completo si y solo si, en el recorrido por niveles, después del primer nulo no aparece ningún nodo real. Equivale a que los nodos ocupen las posiciones contiguas $0, \dots, n-1$ del arreglo implícito, donde los hijos de la posición $i$ están en $2i+1$ y $2i+2$.
- **Ancho máximo:** máximo del tamaño de la cola al comenzar cada nivel (cantidad de nodos reales por nivel).
- **Profundidad mínima:** nivel de la primera hoja que se desencola.
**Justificación formal.** Invariante de la cola: en todo momento contiene nodos de profundidad $d$ seguidos de nodos de profundidad $d+1$ (y nada más). Por inducción: inicialmente solo está la raíz ($d = 0$); al desencolar un nodo de profundidad $d$ se encolan sus hijos, de profundidad $d+1$, al final, lo que mantiene la forma. Por tanto BFS desencola los nodos en orden no decreciente de profundidad. Consecuencias:
 
1. Los nodos de cada nivel salen de la cola de forma contigua, así que el tamaño de la cola al inicio de un nivel es el ancho de ese nivel.
2. La primera hoja desencolada tiene profundidad mínima: si existiera una hoja más superficial, habría salido antes.
3. El orden por niveles coincide con el orden de posiciones del arreglo implícito, lo que permite detectar huecos para la completitud.
DFS no sirve para estas métricas sin información extra, porque visita en orden de profundidad y no de nivel.
 
Complejidad: $O(n)$ en tiempo y $O(w)$ en espacio, con $w$ el ancho máximo.
 
