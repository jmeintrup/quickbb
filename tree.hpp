#ifndef QUICKBB_TREE_HPP
#define QUICKBB_TREE_HPP
#include "_types.hpp"
#include <vector>
#include <memory>
#include <set>
#include <functional>
#include <iostream>

class Tree {
 private:
  tree_data_t _m_data{};
  vertex_index_t _root{};
 public:
  Tree() = default;

  auto begin() {
    return _m_data.begin();
  }

  auto end() {
    return _m_data.end();
  }

  auto begin() const {
    return _m_data.cbegin();
  }

  auto end() const {
    return _m_data.cend();
  }

  tree_node_t &addNode(vertex_index_t v) {
    if (_m_data.find(v) == _m_data.end()) {
      _m_data[v] = tree_node_t();
    }
    return _m_data[v];
  }

  void removeDead() {
    for (auto it = _m_data.begin(); it != _m_data.end(); it++) {
      if (it->second._parent == 0 &&
          it->second._children.empty() &&
          it->second._bag.empty()) {
        _m_data.erase(it);
      }
    }
  }

  tree_node_t &getNode(vertex_index_t v) {
    return _m_data.at(v);
  }

  [[nodiscard]]
  const tree_node_t &getNode(vertex_index_t v) const {
    return _m_data.at(v);
  }

  void setRoot(vertex_index_t v) {
    if (_m_data.find(v) != _m_data.end()) {
      _root = v;
    }
  }

  [[nodiscard]]
  vertex_index_t getRoot() const {
    return _root;
  }

  void connectToParent(vertex_index_t parent, vertex_index_t child) {
    addNode(parent);
    addNode(child);

    _m_data[child]._parent = parent;
    _m_data[parent]._children.insert(child);
  }

  [[nodiscard]]
  size_t order() const {
    return _m_data.size();
  }

  void contractChildToParent(vertex_index_t parent, vertex_index_t child) {
    _m_data[parent]._children.erase(child);
    for (const auto &a : _m_data[child]._children) {
      _m_data[a]._parent = parent;
      _m_data[parent]._children.insert(a);
    }
    for (const auto &a : _m_data[child]._bag) {
      _m_data[parent]._bag.insert(a);
    }
    _m_data.erase(child);
  }

  void dfs(std::function<void(const tree_node_t &, size_t, bool, bool)> post_order,
           std::function<void(const tree_node_t &, size_t, bool, bool)> pre_order)
  const {
    std::function<void(vertex_index_t, size_t, bool)> recursion;
    recursion = [this, &recursion, &post_order, &pre_order]
        (vertex_index_t v, size_t depth, bool last_child) {
      auto n = this->_m_data.at(v);
      pre_order(n, depth, n._children.empty(), last_child);
      auto i = 0;
      for (auto c : n._children) {
        recursion(c, depth + 1, i++ == n._children.size() - 1);
      }
      post_order(n, depth, n._children.empty(), last_child);
    };
    recursion(this->_root, 0, true);
  }

  friend std::ostream &operator<<(std::ostream &os, const Tree &tree);
};

std::ostream &operator<<(std::ostream &os, const Tree &tree) {
  auto pre_order = [&os]
      (const tree_node_t &n, size_t depth, bool is_leaf, bool is_last_child) {
    os << "{";
    if (!is_leaf) {
      os << "\"children\": [";
    }
  };
  auto post_order = [&os]
      (const tree_node_t &n, size_t depth, bool is_leaf, bool is_last_child) {
    if (!is_leaf) {
      os << "], ";
    }
    os << "\"bag\": [";
    auto i = 0;
    for (auto b : n._bag) {
      if (i++ == n._bag.size() - 1) {
        os << b;
      } else {
        os << b << ", ";
      }
    }
    os << "]";
    os << "}";
    if (!is_last_child) {
      os << ",";
    }
  };
  tree.dfs(post_order, pre_order);
  return os;
}

#endif //QUICKBB_TREE_HPP
