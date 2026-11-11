#pragma once
#include <algorithm>
#include <cstddef>
#include <queue>

#include "binary_tree.hpp"

template <typename T>
bool isCompleteTree(const TreeNode<T>* root) {
    std::queue<const TreeNode<T>*> q;
    q.push(root);
    bool seenNull = false;
    while (!q.empty()) {
        const TreeNode<T>* n = q.front();
        q.pop();
        if (!n) {
            seenNull = true;
            continue;
        }
        if (seenNull) return false;
        q.push(n->left);
        q.push(n->right);
    }
    return true;
}

template <typename T>
std::size_t maxWidth(const TreeNode<T>* root) {
    if (!root) return 0;
    std::queue<const TreeNode<T>*> q;
    q.push(root);
    std::size_t best = 0;
    while (!q.empty()) {
        std::size_t levelSize = q.size();
        best = std::max(best, levelSize);
        for (std::size_t i = 0; i < levelSize; ++i) {
            const TreeNode<T>* n = q.front();
            q.pop();
            if (n->left) q.push(n->left);
            if (n->right) q.push(n->right);
        }
    }
    return best;
}

template <typename T>
std::size_t minDepth(const TreeNode<T>* root) {
    if (!root) return 0;
    std::queue<const TreeNode<T>*> q;
    q.push(root);
    std::size_t depth = 0;
    while (!q.empty()) {
        ++depth;
        std::size_t levelSize = q.size();
        for (std::size_t i = 0; i < levelSize; ++i) {
            const TreeNode<T>* n = q.front();
            q.pop();
            if (!n->left && !n->right) return depth;
            if (n->left) q.push(n->left);
            if (n->right) q.push(n->right);
        }
    }
    return depth;
}
