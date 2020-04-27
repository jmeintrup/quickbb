#include <iostream>
#include "graph.hpp"
#include "graph_io.hpp"
#include "quickbb.hpp"

int main() {
    Graph graph;
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(1, 9);
    graph.addEdge(3, 2);
    graph.addEdge(4, 2);
    graph.addEdge(5, 2);
    graph.addEdge(6, 3);
    graph.addEdge(7, 3);
    graph.addEdge(5, 8);
    graph.addEdge(5, 9);

    auto qbb = quickbb(graph);
    write_dot(graph, "graph.dot");
    auto td = td_from_order(graph, qbb.second);
    write_dot(td, "td.dot");

    return 0;
}
