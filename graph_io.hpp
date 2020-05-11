#ifndef QUICKBB_GRAPH_IO_HPP
#define QUICKBB_GRAPH_IO_HPP
#include "graph.hpp"
#include "tree.hpp"
#include <fstream>
#include <set>
#include <sstream>

void write_dot(const Graph &g, std::ostream &os) {
  os << "graph {" << std::endl;
  std::set<std::pair<vertex_index_t, vertex_index_t>> duplicate;
  for (const auto &a : g) {
    auto v = a.first;
    auto neighborhood = a.second;
    for (auto u : neighborhood) {
      if (!duplicate.contains({u, v}) && !duplicate.contains({v, u})) {
        os << "\t" << v << " -- " << u << std::endl;
        duplicate.insert({u, v});
      }

    }
  }
  os << "}" << std::endl;
}
void write_dot(const Tree &t, std::ostream &os) {
  os << "digraph {" << std::endl;
  std::function<void(vertex_index_t)> rec;
  rec = [t, &rec, &os](vertex_index_t id) {
    const auto &node = t.getNode(id);
    os << "\t" << id << " [label = \"{";
    auto i{0};
    for(auto v : node._bag) {
      if(i++ == node._bag.size() - 1) {
        os << v;
      } else {
        os << v << ", ";
      }
    }
    os << "}\"]" << std::endl;
    for(auto c : node._children) {
      rec(c);
    }
    for(auto c : node._children) {
      os << "\t" << id << " -> " << c << std::endl;
    }
  };
  rec(t.getRoot());
  os << "}" << std::endl;
}
void write_json(const Graph &g, const std::string &fileName) {
  std::ofstream file;
  file.open(fileName);
  file << "{" << std::endl;
  std::set<std::pair<vertex_index_t, vertex_index_t>> duplicate;
  size_t count_vertices(0);
  size_t max_order = g.order() - 1;
  for (const auto &a : g) {
    auto v = a.first;
    file << "\t" << '\"' << v << '\"' << ": [";
    auto neighborhood = a.second;
    size_t count = 0;
    size_t max = neighborhood.size() - 1;
    for (auto u : neighborhood) {
      if (count++ == max) {
        file << u;
      } else {
        file << u << ", ";
      }
    }
    if (count_vertices++ == max_order) {
      file << "]" << std::endl;
    } else {
      file << "]," << std::endl;
    }
  }
  file << "}" << std::endl;
  file.close();
}

void write_pace(const Graph &g, const std::string &fileName) {
  std::ofstream file;
  file.open(fileName);
  size_t edge_count(0);
  for (const auto &a : g) {
    edge_count += a.second.size();
  }
  edge_count /= 2;

  file << "p td " << g.order() << ' ' << edge_count << std::endl;
  std::set<std::pair<vertex_index_t, vertex_index_t>> duplicate;
  for (const auto &a : g) {
    auto v = a.first;
    auto neighborhood = a.second;
    for (auto u : neighborhood) {
      if (!duplicate.contains({u, v}) && !duplicate.contains({v, u})) {
        file << u << ' ' << v << std::endl;
        duplicate.insert({u, v});
      }

    }
  }
  file.close();
}

void write_pace(const Tree &t, size_t tw, size_t order, std::ostream &os) {
  os << "s td" << " " << t.order() << " " << tw << " " << order << std::endl;
  for(const auto& n : t) {
    os << n.first << " ";
    size_t i{0};
    for(const auto &v : n.second._bag) {
      if(i == n.second._bag.size() - 1) {
        os << v;
      } else {
        os << v << " ";
      }
    }
    os << std::endl;
  }
  for(const auto& n : t) {
    if(!n.second._children.empty()) {
      for(const auto &v : n.second._children) {
        os << n.first << " " << v << std::endl;
      }
    }
  }
}

Graph read_pace(std::istream& in) {
  Graph graph;

  for (std::string line; std::getline(in, line);) {
    if(line[0] == 'c' || line[0] == 'p' || line[0] == '#') {
      continue;
    } else {
      vertex_index_t u;
      vertex_index_t v;
      std::stringstream ss;
      ss<<line;
      ss>>u>>v;
      graph.addEdge(u, v);
    }
  }
  return graph;
}

#endif //QUICKBB_GRAPH_IO_HPP
