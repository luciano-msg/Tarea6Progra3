#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "bfs.hpp"
#include "binary_tree.hpp"
#include "bst.hpp"
#include "dfs.hpp"
#include "rbtree.hpp"

namespace {

int failures = 0;

void check(bool ok, const std::string& what) {
    std::cout << (ok ? "  [OK]   " : "  [FAIL] ") << what << '\n';
    if (!ok) ++failures;
}

template <typename T>
void printVec(const std::string& label, const std::vector<T>& v) {
    std::cout << label << ": ";
    for (std::size_t i = 0; i < v.size(); ++i) std::cout << (i ? " " : "") << v[i];
    std::cout << '\n';
}

std::vector<int> range(int a, int b) {
    std::vector<int> v(b - a + 1);
    std::iota(v.begin(), v.end(), a);
    return v;
}

std::size_t rbBound(std::size_t n) {
    return static_cast<std::size_t>(std::floor(2.0 * std::log2(static_cast<double>(n) + 1.0)));
}

void ejercicio1() {
    std::cout << "\n=== Ejercicio 1: BST iterativo y degradacion ===\n";

    BST<int> bst;
    for (int k : range(1, 20)) bst.insert(k);
    std::cout << "Insertados 20 elementos (1..20 en orden ascendente)\n";
    std::cout << "Altura antes de eliminar: " << bst.height() << '\n';

    check(bst.remove(7) && bst.remove(13), "eliminar 7 y 13");
    check(!bst.remove(99), "eliminar una clave inexistente devuelve false");

    std::vector<int> in = bst.inorder();
    printVec("In-order (std::stack)", in);
    std::cout << "Tamano: " << bst.size() << "  Altura final: " << bst.height() << '\n';
    check(in.size() == 18 && std::is_sorted(in.begin(), in.end()) && !bst.contains(7) &&
              !bst.contains(13),
          "in-order ordenado, 18 elementos, sin 7 ni 13");
    check(bst.height() == 18, "altura 18: el arbol sigue siendo una cadena");

    std::cout << "\nEliminacion con dos hijos:\n";
    BST<int> t;
    for (int k : {50, 30, 70, 20, 40, 60, 80, 35, 45}) t.insert(k);
    check(t.remove(30), "eliminar 30 (dos hijos)");
    check(t.remove(50), "eliminar 50 (raiz con dos hijos)");
    std::vector<int> tin = t.inorder();
    printVec("In-order", tin);
    check(tin == std::vector<int>({20, 35, 40, 45, 60, 70, 80}), "in-order correcto tras eliminar");
    printVec("Pre-order", t.preorder());

    std::mt19937 rng(42);
    std::vector<int> shuffled = range(1, 20);
    std::shuffle(shuffled.begin(), shuffled.end(), rng);
    BST<int> rnd;
    for (int k : shuffled) rnd.insert(k);
    std::cout << "\nMismos 20 datos en orden aleatorio: altura = " << rnd.height()
              << " (vs 20 con datos ordenados)\n";
}

void problema2() {
    std::cout << "\n=== Problema 2: Red-Black Tree ===\n";

    RedBlackTree<int> rb;
    BST<int> bst;
    for (int k : range(1, 20)) {
        rb.insert(k);
        bst.insert(k);
    }

    std::cout << "Pre-orden del Red-Black Tree (clave(color)):\n  ";
    for (auto& [key, c] : rb.preorder()) std::cout << key << '(' << c << ") ";
    std::cout << '\n';
    printVec("Pre-orden del BST", bst.preorder());

    std::string why;
    bool ok = rb.verify(&why);
    check(ok, ok ? "las 5 propiedades del Red-Black Tree se cumplen" : why);
    check(!rb.insert(10), "insertar duplicado devuelve false");

    std::cout << "\nAltura con 20 datos ordenados:  BST = " << bst.height()
              << "   Red-Black = " << rb.height() << '\n';
    check(rb.height() <= rbBound(20), "altura RB <= 2*log2(n+1) = " + std::to_string(rbBound(20)));

    std::cout << "\nComparacion experimental (inserciones ascendentes):\n";
    std::cout << std::setw(8) << "n" << std::setw(12) << "BST" << std::setw(12) << "RB"
              << std::setw(18) << "cota 2*log2(n+1)" << '\n';
    for (int n : {20, 100, 1000, 10000}) {
        BST<int> b;
        RedBlackTree<int> r;
        for (int k = 1; k <= n; ++k) {
            b.insert(k);
            r.insert(k);
        }
        std::cout << std::setw(8) << n << std::setw(12) << b.height() << std::setw(12)
                  << r.height() << std::setw(18) << rbBound(n) << '\n';
        check(b.height() == static_cast<std::size_t>(n) && r.verify() &&
                  r.height() <= rbBound(n),
              "n = " + std::to_string(n) + ": BST degenerado, RB valido y dentro de la cota");
    }

    std::mt19937 rng(7);
    std::vector<int> data = range(1, 5000);
    std::shuffle(data.begin(), data.end(), rng);
    RedBlackTree<int> rnd;
    for (int k : data) rnd.insert(k);
    check(rnd.verify(&why) && rnd.size() == 5000, "5000 inserciones aleatorias: propiedades validas");
    std::cout << "  altura = " << rnd.height() << " (cota " << rbBound(5000) << ")\n";
}

void problema3() {
    std::cout << "\n=== Problema 3: DFS iterativo (suma raiz-hoja) ===\n";
    using O = std::optional<int>;
    constexpr auto N = std::nullopt;

    TreeNode<int>* t = buildFromLevelOrder<int>({O(5), O(4), O(8), O(11), N, O(13), O(4), O(7), O(2),
                                                 N, N, N, O(1)});
    for (long long k : {22LL, 26LL, 18LL, 100LL}) {
        auto path = rootToLeafPathSum(t, k);
        if (path) printVec("K = " + std::to_string(k) + " -> camino", *path);
        else std::cout << "K = " << k << " -> no existe camino\n";
    }
    check(rootToLeafPathSum(t, 22) == std::vector<int>({5, 4, 11, 2}), "K=22 devuelve 5 4 11 2");
    check(rootToLeafPathSum(t, 26) == std::vector<int>({5, 8, 13}), "K=26 devuelve 5 8 13");
    check(!rootToLeafPathSum(t, 100).has_value(), "K=100 no existe");
    check(!rootToLeafPathSum(t, 9).has_value(), "K=9 no existe (5+4 no termina en hoja)");
    freeTree(t);

    TreeNode<int>* neg = buildFromLevelOrder<int>({O(-2), N, O(-3)});
    check(rootToLeafPathSum(neg, -5) == std::vector<int>({-2, -3}), "valores negativos: K=-5");
    freeTree(neg);

    TreeNode<int>* single = buildFromLevelOrder<int>({O(7)});
    check(rootToLeafPathSum(single, 7) == std::vector<int>({7}), "arbol de un nodo");
    freeTree(single);

    check(!rootToLeafPathSum<int>(nullptr, 0).has_value(), "arbol vacio");
}

void problema4() {
    std::cout << "\n=== Problema 4: BFS iterativo (completo, ancho, profundidad minima) ===\n";
    using O = std::optional<int>;
    constexpr auto N = std::nullopt;

    struct Caso {
        std::string nombre;
        std::vector<O> datos;
        bool completo;
        std::size_t ancho;
        std::size_t prof;
    };

    std::vector<Caso> casos = {
        {"completo [1,2,3,4,5,6]", {O(1), O(2), O(3), O(4), O(5), O(6)}, true, 3, 3},
        {"perfecto [1..7]", {O(1), O(2), O(3), O(4), O(5), O(6), O(7)}, true, 4, 3},
        {"hueco [1,2,3,4,5,N,7]", {O(1), O(2), O(3), O(4), O(5), N, O(7)}, false, 3, 3},
        {"cadena derecha [1,N,2,N,3]", {O(1), N, O(2), N, O(3)}, false, 1, 3},
        {"hoja izquierda tardia [1,2,3,N,4]", {O(1), O(2), O(3), N, O(4)}, false, 2, 2},
        {"un nodo [1]", {O(1)}, true, 1, 1},
        {"ejemplo DFS", {O(5), O(4), O(8), O(11), N, O(13), O(4), O(7), O(2), N, N, N, O(1)},
         false, 3, 3},
    };

    for (const Caso& c : casos) {
        TreeNode<int>* t = buildFromLevelOrder<int>(c.datos);
        bool comp = isCompleteTree(t);
        std::size_t w = maxWidth(t);
        std::size_t d = minDepth(t);
        std::cout << c.nombre << ": completo=" << (comp ? "si" : "no") << " ancho=" << w
                  << " profMin=" << d << '\n';
        check(comp == c.completo && w == c.ancho && d == c.prof, c.nombre);
        freeTree(t);
    }

    check(isCompleteTree<int>(nullptr) && maxWidth<int>(nullptr) == 0 && minDepth<int>(nullptr) == 0,
          "arbol vacio: completo, ancho 0, profundidad 0");
}

}  // namespace

int main() {
    ejercicio1();
    problema2();
    problema3();
    problema4();

    std::cout << "\n" << (failures == 0 ? "Todas las verificaciones pasaron."
                                        : "Hubo verificaciones fallidas.")
              << '\n';
    return failures == 0 ? 0 : 1;
}
