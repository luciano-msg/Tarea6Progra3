#pragma once
#include <cstddef>
#include <stack>
#include <string>
#include <utility>
#include <vector>

enum class Color { RED, BLACK };

template <typename T>
class RedBlackTree {
    struct Node {
        T key;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
    };

    Node* nil_;
    Node* root_;
    std::size_t size_ = 0;

    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil_) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_) root_ = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != nil_) x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == nil_) root_ = x;
        else if (y == y->parent->left) y->parent->left = x;
        else y->parent->right = x;
        x->right = y;
        y->parent = x;
    }

    void fixInsert(Node* z) {
        while (z->parent->color == Color::RED) {
            Node* p = z->parent;
            Node* g = p->parent;
            if (p == g->left) {
                Node* uncle = g->right;
                if (uncle->color == Color::RED) {
                    p->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    g->color = Color::RED;
                    z = g;
                } else {
                    if (z == p->right) {
                        z = p;
                        rotateLeft(z);
                        p = z->parent;
                    }
                    p->color = Color::BLACK;
                    g->color = Color::RED;
                    rotateRight(g);
                }
            } else {
                Node* uncle = g->left;
                if (uncle->color == Color::RED) {
                    p->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    g->color = Color::RED;
                    z = g;
                } else {
                    if (z == p->left) {
                        z = p;
                        rotateRight(z);
                        p = z->parent;
                    }
                    p->color = Color::BLACK;
                    g->color = Color::RED;
                    rotateLeft(g);
                }
            }
        }
        root_->color = Color::BLACK;
    }

    int blackHeight(const Node* n, std::string& msg) const {
        if (n == nil_) return 1;
        if (n->color == Color::RED &&
            (n->left->color == Color::RED || n->right->color == Color::RED)) {
            msg = "Propiedad 3: nodo rojo con hijo rojo";
            return -1;
        }
        if ((n->left != nil_ && n->left->parent != n) ||
            (n->right != nil_ && n->right->parent != n)) {
            msg = "Enlace parent inconsistente";
            return -1;
        }
        int l = blackHeight(n->left, msg);
        if (l < 0) return -1;
        int r = blackHeight(n->right, msg);
        if (r < 0) return -1;
        if (l != r) {
            msg = "Propiedad 4: caminos con distinta cantidad de nodos negros";
            return -1;
        }
        return l + (n->color == Color::BLACK ? 1 : 0);
    }

public:
    RedBlackTree() {
        nil_ = new Node{T(), Color::BLACK, nullptr, nullptr, nullptr};
        nil_->left = nil_->right = nil_->parent = nil_;
        root_ = nil_;
    }
    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;

    ~RedBlackTree() {
        std::stack<Node*> st;
        if (root_ != nil_) st.push(root_);
        while (!st.empty()) {
            Node* n = st.top();
            st.pop();
            if (n->left != nil_) st.push(n->left);
            if (n->right != nil_) st.push(n->right);
            delete n;
        }
        delete nil_;
    }

    bool insert(const T& key) {
        Node* parent = nil_;
        Node* cur = root_;
        while (cur != nil_) {
            parent = cur;
            if (key < cur->key) cur = cur->left;
            else if (cur->key < key) cur = cur->right;
            else return false;
        }
        Node* z = new Node{key, Color::RED, nil_, nil_, parent};
        if (parent == nil_) root_ = z;
        else if (key < parent->key) parent->left = z;
        else parent->right = z;
        ++size_;
        fixInsert(z);
        return true;
    }

    bool contains(const T& key) const {
        const Node* cur = root_;
        while (cur != nil_) {
            if (key < cur->key) cur = cur->left;
            else if (cur->key < key) cur = cur->right;
            else return true;
        }
        return false;
    }

    std::size_t height() const {
        std::size_t best = 0;
        std::stack<std::pair<const Node*, std::size_t>> st;
        if (root_ != nil_) st.push({root_, 1});
        while (!st.empty()) {
            auto [n, d] = st.top();
            st.pop();
            if (d > best) best = d;
            if (n->left != nil_) st.push({n->left, d + 1});
            if (n->right != nil_) st.push({n->right, d + 1});
        }
        return best;
    }

    std::vector<std::pair<T, char>> preorder() const {
        std::vector<std::pair<T, char>> out;
        std::stack<const Node*> st;
        if (root_ != nil_) st.push(root_);
        while (!st.empty()) {
            const Node* n = st.top();
            st.pop();
            out.push_back({n->key, n->color == Color::RED ? 'R' : 'B'});
            if (n->right != nil_) st.push(n->right);
            if (n->left != nil_) st.push(n->left);
        }
        return out;
    }

    bool verify(std::string* why = nullptr) const {
        auto fail = [&](const std::string& m) {
            if (why) *why = m;
            return false;
        };
        if (nil_->color != Color::BLACK) return fail("Propiedad 5: la hoja nula debe ser negra");
        if (root_ != nil_) {
            if (root_->color != Color::BLACK) return fail("Propiedad 2: la raiz debe ser negra");
            if (root_->parent != nil_) return fail("La raiz debe tener parent nulo");
        }

        std::stack<const Node*> st;
        const Node* cur = root_;
        const Node* prev = nullptr;
        std::size_t count = 0;
        while (cur != nil_ || !st.empty()) {
            while (cur != nil_) {
                st.push(cur);
                cur = cur->left;
            }
            cur = st.top();
            st.pop();
            if (prev && !(prev->key < cur->key)) return fail("Se viola el orden de BST");
            prev = cur;
            ++count;
            cur = cur->right;
        }
        if (count != size_) return fail("Cantidad de nodos distinta de size");

        std::string msg;
        if (blackHeight(root_, msg) < 0) return fail(msg);
        return true;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
};
