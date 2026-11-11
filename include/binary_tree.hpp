#pragma once
#include <cstddef>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

template <typename T>
struct TreeNode {
    T val;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    explicit TreeNode(const T& v) : val(v) {}
};

template <typename T>
TreeNode<T>* buildFromLevelOrder(const std::vector<std::optional<T>>& v) {
    if (v.empty() || !v[0]) return nullptr;
    auto* root = new TreeNode<T>(*v[0]);
    std::queue<TreeNode<T>*> q;
    q.push(root);
    std::size_t i = 1;
    while (!q.empty() && i < v.size()) {
        TreeNode<T>* n = q.front();
        q.pop();
        if (i < v.size()) {
            if (v[i]) {
                n->left = new TreeNode<T>(*v[i]);
                q.push(n->left);
            }
            ++i;
        }
        if (i < v.size()) {
            if (v[i]) {
                n->right = new TreeNode<T>(*v[i]);
                q.push(n->right);
            }
            ++i;
        }
    }
    return root;
}

template <typename T>
void freeTree(TreeNode<T>* root) {
    std::stack<TreeNode<T>*> st;
    if (root) st.push(root);
    while (!st.empty()) {
        TreeNode<T>* n = st.top();
        st.pop();
        if (n->left) st.push(n->left);
        if (n->right) st.push(n->right);
        delete n;
    }
}
