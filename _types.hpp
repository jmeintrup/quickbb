#ifndef QUICKBB__TYPES_HPP
#define QUICKBB__TYPES_HPP

#include <vector>
#include <cstdint>
#include <map>
#include <set>

typedef size_t vertex_index_t;
typedef std::vector<vertex_index_t> adj_arr_t;
typedef std::map<vertex_index_t, adj_arr_t> graph_data_t;
typedef std::set<vertex_index_t> bag_t;
typedef std::set<vertex_index_t> node_children_t;
struct tree_node_t {
  bag_t _bag{};
  node_children_t _children{};
  vertex_index_t _parent{};
  tree_node_t() = default;
};
typedef std::map<vertex_index_t, tree_node_t> tree_data_t;

#endif //QUICKBB__TYPES_HPP
