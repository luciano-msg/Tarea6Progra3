#pragma once
#include <cstddef>
#include <stack>
#include <utility>
#include <vector>

template <typename T>
class BST {
    struct Node {
        T key;
        Node* left;
        Node* right;
        explicit Node(const T& k) : key(k), left(nullptr), right(nullptr) {}
    };

    Node* root_ = nullptr;
    std::size_t size_ = 0;

public:
    BST() = default;
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;
    ~BST() { clear(); }

    void clear() {
        std::stack<Node*> st;
        if (root_) st.push(root_);
        while (!st.empty()) {
            Node* n = st.top();
            st.pop();
            if (n->left) st.push(n->left);
            if (n->right) st.push(n->right);
            delete n;
        }
        root_ = nullptr;
        size_ = 0;
    }

    bool insert(const T& key) {
        if (!root_) {
            root_ = new Node(key);
            ++size_;
            return true;
        }
        Node* cur = root_;
        while (true) {
            if (key < cur->key) {
                if (!cur->left) {
                    cur->left = new Node(key);
                    break;
                }
                cur = cur->left;
            } else if (cur->key < key) {
                if (!cur->right) {
                    cur->right = new Node(key);
                    break;
                }
                cur = cur->right;
            } else {
                return false;
            }
        }
        ++size_;
        return true;
    }

    bool contains(const T& key) const {
        Node* cur = root_;
        while (cur) {
            if (key < cur->key) cur = cur->left;
            else if (cur->key < key) cur = cur->right;
            else return true;
        }
        return false;
    }

    bool remove(const T& key) {
        Node* parent = nullptr;
        Node* cur = root_;
        while (cur && (key < cur->key || cur->key < key)) {
            parent = cur;
            cur = key < cur->key ? cur->left : cur->right;
        }
        if (!cur) return false;

        if (cur->left && cur->right) {
            Node* succParent = cur;
            Node* succ = cur->right;
            while (succ->left) {
                succParent = succ;
                succ = succ->left;
            }
            cur->key = succ->key;
            parent = succParent;
            cur = succ;
        }

        Node* child = cur->left ? cur->left : cur->right;
        if (!parent) root_ = child;
        else if (parent->left == cur) parent->left = child;
        else parent->right = child;

        delete cur;
        --size_;
        return true;
    }

    std::vector<T> inorder() const {
        std::vector<T> out;
        std::stack<Node*> st;
        Node* cur = root_;
        while (cur || !st.empty()) {
            while (cur) {
                st.push(cur);
                cur = cur->left;
            }
            cur = st.top();
            st.pop();
            out.push_back(cur->key);
            cur = cur->right;
        }
        return out;
    }

    std::vector<T> preorder() const {
        std::vector<T> out;
        std::stack<Node*> st;
        if (root_) st.push(root_);
        while (!st.empty()) {
            Node* n = st.top();
            st.pop();
            out.push_back(n->key);
            if (n->right) st.push(n->right);
            if (n->left) st.push(n->left);
        }
        return out;
    }

    std::size_t height() const {
        std::size_t best = 0;
        std::stack<std::pair<Node*, std::size_t>> st;
        if (root_) st.push({root_, 1});
        while (!st.empty()) {
            auto [n, d] = st.top();
            st.pop();
            if (d > best) best = d;
            if (n->left) st.push({n->left, d + 1});
            if (n->right) st.push({n->right, d + 1});
        }
        return best;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
};
