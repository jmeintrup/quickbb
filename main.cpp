#include <iostream>
#include "graph.hpp"
#include "graph_io.hpp"
#include "quickbb.hpp"

constexpr char PROGRAM_NAME[] = "quickbb";

void print_help() {
  std::cout << PROGRAM_NAME << "[options] < file" << std::endl <<
            "Reads a Graph G in .gr format and writes a tree-decomposition of G to stdout" << std::endl <<
            "Options:" << std::endl <<
            "-h | --help               Print this help" << std::endl <<
            "-t | --time <time>        Sets maximum timeout in seconds. Defaults to 360." << std::endl <<
            "-o | --output <file>      Specifies output file. If none given, outputs to stdout" << std::endl <<
            "-i | --input <file>       Specifies input file. If none given, reads from stdin" << std::endl;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args;
  size_t alloted_time = 360;
  for (auto i = 1; i < argc; i++) {
    args.emplace_back(argv[i]);
  }
  auto help_pred = [](const std::string &a) {
    return a == "-h" || a == "--help";
  };
  auto help = std::find_if(args.begin(), args.end(), help_pred) != args.end();
  if (help) {
    print_help();
    return 0;
  }

  auto time_pred = [](const std::string &a) {
    return a == "-t" || a == "--time";
  };

  auto time = std::find_if(args.begin(), args.end(), time_pred);
  if (time != args.end() && ++time != args.end()) {
    alloted_time = std::stoi(*time);
  }

  auto output_pred = [](const std::string &a) {
    return a == "-o" || a == "--output";
  };

  auto output_file = std::find_if(args.begin(), args.end(), output_pred);

  auto has_output_file = false;
  std::ofstream output_file_stream;
  if (output_file != args.end() && ++output_file != args.end()) {
    output_file_stream.open(*output_file);
    has_output_file = true;
  }

  auto input_pred = [](const std::string &a) {
    return a == "-i" || a == "--input";
  };

  auto input_file = std::find_if(args.begin(), args.end(), input_pred);

  auto has_input_file = false;
  std::ifstream input_file_stream;
  if (input_file != args.end() && ++input_file != args.end()) {
    input_file_stream.open(*input_file);
    has_input_file = true;
  }

  Graph graph;
  graph = read_pace(has_input_file ? input_file_stream : std::cin);
  auto[tw, elimination_order] = quickbb(graph, alloted_time);
  auto t = td_from_order(graph, elimination_order);
  write_pace(t, tw, graph.order(), has_output_file ? output_file_stream : std::cout);
  return 0;
}
