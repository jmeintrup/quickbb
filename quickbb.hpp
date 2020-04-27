#ifndef QUICKBB_QUICKBB_HPP
#define QUICKBB_QUICKBB_HPP

#include <set>
#include <utility>
#include "graph.hpp"
#include "_types.hpp"
#include "tree.hpp"

void make_clique(Graph &graph, const adj_arr_t &vertices) {
  for (auto u : vertices) {
    for (auto v : vertices) {
      if (u != v) graph.addEdge(u, v);
    }
  }
}

bool is_clique(const Graph &graph, const adj_arr_t &vertices) {
  for (auto u : vertices) {
    for (auto v : vertices) {
      if (u != v && !graph.hasEdge(u, v)) return false;
    }
  }
  return true;
}

bool simplicial(const Graph &graph, vertex_index_t vertex) {
  return is_clique(graph, graph.getNeighborhood(vertex));
}

bool almost_simplicial(const Graph &graph, vertex_index_t vertex) {
  const adj_arr_t &neighbors = graph.getNeighborhood(vertex);

  for (auto v : neighbors) {
    adj_arr_t temp(neighbors);
    temp.erase(std::find(temp.begin(), temp.end(), v));
    if (is_clique(graph, temp)) return true;
  }
  return false;
}

void eliminate(Graph &graph, vertex_index_t vertex) {
  make_clique(graph, graph.getNeighborhood(vertex));
  graph.removeVertex(vertex);
}

size_t count_fillin(const Graph &graph, adj_arr_t vertices) {
  size_t count = 0;
  for (auto u : vertices) {
    for (auto v : vertices) {
      if (u != v) {
        auto u_nb = graph.getNeighborhood(u);
        auto found = std::find(std::begin(u_nb), std::end(u_nb), v);
        if (found == std::end(vertices)) {
          count++;
        }
      }
    }
  }
  return count / 2;
}

std::pair<adj_arr_t, size_t> upper_bound(const Graph &graph) {
  Graph graph_copy(graph);
  size_t max_degree(0);
  adj_arr_t ordered_vertices;
  using value_t = decltype(graph_copy.begin())::value_type;

  while (graph_copy.order() > 0) {
    auto cmp = [graph_copy](const value_t &u, const value_t &v) {
      return count_fillin(graph_copy, u.second) < count_fillin(graph_copy, v.second);
    };

    auto u = std::min_element(std::begin(graph_copy), std::end(graph_copy), cmp);
    max_degree = std::max(u->second.size(), max_degree);

    eliminate(graph_copy, u->first);
    ordered_vertices.emplace_back(u->first);
  }
  return {ordered_vertices, max_degree};
}

size_t lower_bound(const Graph &graph) {
  Graph graph_copy(graph);
  size_t max_degree(0);
  using value_t = decltype(graph_copy.begin())::value_type;

  while (graph_copy.order() > 0) {
    auto u = std::min_element(
        std::begin(graph_copy),
        std::end(graph_copy),
        [](const value_t &u, const value_t &v) {
          return u.second.size() < v.second.size();
        }
    );
    max_degree = std::max(u->second.size(), max_degree);

    auto neighbors = graph_copy.getNeighborhood(u->first);

    auto cmp = [graph_copy, neighbors](vertex_index_t u, vertex_index_t v) {
      auto u_nb = graph_copy.getNeighborhood(u);
      std::set<vertex_index_t> u_tmp(std::begin(u_nb), std::end(u_nb));
      u_tmp.insert(std::begin(neighbors), std::end(neighbors));

      auto v_nb = graph_copy.getNeighborhood(v);
      std::set<vertex_index_t> v_tmp(std::begin(v_nb), std::end(v_nb));
      v_tmp.insert(std::begin(neighbors), std::end(neighbors));

      return u_tmp.size() < v_tmp.size();
    };

    if (!neighbors.empty()) {
      auto v = std::min_element(
          std::begin(neighbors),
          std::end(neighbors),
          cmp);
      graph_copy.contract_edge(u->first, *v);
    } else {
      graph_copy.removeVertex(u->first);
    }
  }
  return max_degree;
}

std::pair<size_t, adj_arr_t> quickbb(Graph graph) {
  auto upper_bound_pair =
      upper_bound(graph);
  auto lb =
      lower_bound(graph);

  auto best_order = upper_bound_pair.first;
  auto best_upper_bound = upper_bound_pair.second;

  size_t best_count = 0;

  adj_arr_t order;

  std::function<void(Graph &, adj_arr_t, size_t, size_t)> bb;

  bb = [&bb,
      &best_upper_bound,
      &best_order,
      lb,
      best_count]
      (Graph &graph, adj_arr_t order, size_t f, size_t g) mutable {
    best_count++;
    if (graph.order() < 2 && f < best_upper_bound) {
      assert(f == g);
      best_upper_bound = f;
      best_order = adj_arr_t(order);
      for (const auto &v : graph) {
        best_order.emplace_back(v.first);
      }
    } else {
      adj_arr_t vertices;
      for (const auto &a : graph) {
        if (simplicial(graph, a.first) ||
            (almost_simplicial(graph, a.first) && a.second.size() <= lb)) {
          vertices.clear();
          vertices.push_back(a.first);
          break;
        } else {
          vertices.push_back(a.first);
        }
      }
      for (auto v : vertices) {
        auto next_graph(graph);
        eliminate(next_graph, v);
        auto next_order(order);
        next_order.emplace_back(v);
        auto next_g = std::max(g, graph.getNeighborhood(v).size());
        auto next_f = std::max(g, lower_bound(next_graph));
        if (next_f < best_upper_bound) {
          bb(next_graph, next_order, next_f, next_g);
        }
      }
    }

  };

  if (lb < best_upper_bound) {
    bb(graph, order, lb, 0);
  }
  return {best_upper_bound, best_order};
}

Tree td_from_order(const Graph& graph, const std::vector<vertex_index_t>& order) {
  Tree tree;
  auto graph_copy(graph);
  std::vector<std::pair<vertex_index_t , std::set<vertex_index_t >>> stack;

  for(auto v : order) {
    auto nb = graph_copy.getNeighborhood(v);
    stack.emplace_back(
        std::make_pair(v, std::set<vertex_index_t>{nb.begin(), nb.end()})
        );
    eliminate(graph_copy, v);
  }

  for(auto it = stack.rbegin(); it != stack.rend(); it++) {
    auto tmp = *it;
    if(tree.order() == 0) {
      auto &v = tree.addNode(tmp.first);
      v._bag = std::set<vertex_index_t >{tmp.second};
      v._bag.insert(tmp.first);
      tree.setRoot(tmp.first);
    } else {
      for(const auto& w : tree) {
        auto sub_set = true;
        for(auto v : tmp.second) {
          if(!w.second._bag.contains(v)) {
            sub_set = false;
            break;
          }
        }
        if(sub_set) {
          auto &v = tree.addNode(tmp.first);
          v._bag = std::set<vertex_index_t >{tmp.second};
          v._bag.insert(tmp.first);
          tree.connectToParent(w.first, tmp.first);
          break;
        }
      }
    }
  }
  return tree;
}

#endif //QUICKBB_QUICKBB_HPP
