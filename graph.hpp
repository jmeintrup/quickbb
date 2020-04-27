#ifndef QUICKBB_GRAPH_HPP
#define QUICKBB_GRAPH_HPP
#include <algorithm>
#include <assert.h>
#include <iostream>
#include "_types.hpp"

class Graph {
 private:
  graph_data_t m_data_;
 public:
  Graph() = default;

  [[nodiscard]]
  const adj_arr_t &getNeighborhood(vertex_index_t nodeIndex) const {
    return m_data_.at(nodeIndex);
  }

  [[nodiscard]]
  auto begin() const {
    return m_data_.begin();
  }

  [[nodiscard]]
  auto end() const {
    return m_data_.end();
  }

  void removeVertex(vertex_index_t vertexIndex) {
    std::vector<vertex_index_t> to_be_removed;
    to_be_removed.emplace_back(vertexIndex);
    for (const auto &v : m_data_) {
      if (v.first == vertexIndex) continue;

      auto pred = [vertexIndex](vertex_index_t v) { return vertexIndex == v; };
      auto remove_it = std::remove_if(m_data_[v.first].begin(),
                                      m_data_[v.first].end(),
                                      pred);
      m_data_[v.first].erase(remove_it, m_data_[v.first].end());
      if (degree(v.first) == 0) {
        to_be_removed.emplace_back(v.first);
      }
    }
    for (const auto &v : to_be_removed) {
      m_data_.erase(m_data_.find(v));
    }
  }

  [[nodiscard]]
  bool hasEdge(vertex_index_t u, vertex_index_t v) const {
    if (m_data_.count(v) == 0 || m_data_.count(u) == 0) return false;
    auto result = std::find(m_data_.at(u).begin(), m_data_.at(u).end(), v);
    return result != m_data_.at(u).end();
  }

  bool addEdge(vertex_index_t u, vertex_index_t v) {
    if (hasEdge(u, v)) return false;
    m_data_[u].emplace_back(v);
    m_data_[v].emplace_back(u);
    return true;
  }

  bool removeEdge(vertex_index_t u, vertex_index_t v) {
    if (!hasEdge(u, v)) return false;

    auto u_begin = m_data_[u].begin();
    auto u_end = m_data_[u].end();
    m_data_[u].erase(std::remove(u_begin, u_end, v), u_end);
    if (degree(u) == 0) {
      m_data_.erase(m_data_.find(u));
    }

    auto v_begin = m_data_[v].begin();
    auto v_end = m_data_[v].end();
    m_data_[v].erase(std::remove(v_begin, v_end, u), v_end);
    if (degree(v) == 0) {
      m_data_.erase(m_data_.find(v));
    }
    return true;
  }

  void contract_edge(vertex_index_t u, vertex_index_t v) {
    m_data_[u].erase(std::find(m_data_[u].begin(), m_data_[u].end(), v));
    const auto u_deg = m_data_[u].size();
    for (auto n : m_data_[v]) {
      if (n == u) continue;
      auto last = m_data_[u].begin() + u_deg;
      auto found = std::find(m_data_[u].begin(), last, n);

      if (found != last) {
        m_data_[n].erase(std::find(m_data_[n].begin(), m_data_[n].end(), v));
      } else {
        m_data_[u].emplace_back(n);
        for (auto &a : m_data_[n]) {
          if (a == v) {
            a = u;
            break;
          }
        }
      }
    }
    m_data_.erase(v);
  }

  [[nodiscard]]
  vertex_index_t degree(vertex_index_t nodeIndex) const {
    return m_data_.at(nodeIndex).size();
  }

  [[nodiscard]]
  vertex_index_t order() const {
    return m_data_.size();
  }

  std::vector<std::vector<bool>> to_adj_matrix() {
    using value_t = decltype(m_data_)::value_type;
    struct {
      bool operator()(const value_t &u, const value_t &v) {
        return u.first > v.first;
      }
    } comp;
    auto dim = std::max_element(std::begin(m_data_), std::end(m_data_), comp)->first;
    std::vector<std::vector<bool>> adj_matrix;
    for (int i = 0; i < dim; i++) {
      adj_matrix.emplace_back(std::vector<bool>(dim));
    }

    for (const auto &u : m_data_) {
      for (auto v : u.second) {
        adj_matrix[u.first][v] = true;
      }
    }

    return adj_matrix;
  }

  friend std::ostream &operator<<(std::ostream &os, const Graph &graph);
};

std::ostream &operator<<(std::ostream &os, const Graph &graph) {
  os << "Order: " << graph.order() << std::endl;

  for (auto it = graph.m_data_.begin(); it != graph.m_data_.end(); ++it) {
    os << it->first << ": [";
    for (auto j = 0; j < graph.degree(it->first) - 1; j++) {
      os << graph.getNeighborhood(it->first)[j] << ", ";
    }
    os << graph.getNeighborhood(it->first)[graph.degree(it->first) - 1] << "]" << std::endl;
  }
  return os;
}

#endif //QUICKBB_GRAPH_HPP
