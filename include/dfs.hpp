#pragma once
#include <algorithm>
#include <optional>
#include <stack>
#include <vector>

#include "binary_tree.hpp"

template <typename T>
std::optional<std::vector<T>> rootToLeafPathSum(const TreeNode<T>* root, long long k) {
    if (!root) return std::nullopt;

    struct Frame {
        const TreeNode<T>* node;
        long long sum;
        int state;
    };

    std::stack<Frame> st;
    st.push({root, static_cast<long long>(root->val), 0});

    while (!st.empty()) {
        Frame& f = st.top();
        if (f.state == 0) {
            f.state = 1;
            if (!f.node->left && !f.node->right && f.sum == k) {
                std::vector<T> path;
                while (!st.empty()) {
                    path.push_back(st.top().node->val);
                    st.pop();
                }
                std::reverse(path.begin(), path.end());
                return path;
            }
            if (f.node->left) {
                const TreeNode<T>* c = f.node->left;
                long long s = f.sum + static_cast<long long>(c->val);
                st.push({c, s, 0});
            }
        } else if (f.state == 1) {
            f.state = 2;
            if (f.node->right) {
                const TreeNode<T>* c = f.node->right;
                long long s = f.sum + static_cast<long long>(c->val);
                st.push({c, s, 0});
            }
        } else {
            st.pop();
        }
    }
    return std::nullopt;
}
