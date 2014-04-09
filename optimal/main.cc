#include "routage.hh"
#include <cstdio>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <ctime>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  srand(time(NULL));
  // Parse options
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("input,i", po::value<string>(), "input file")
    ("map,m", po::value<string>(), "map file")
    ("output,o", po::value<string>(), "output file")
    ("correct,c", "improve")
    ("prune,p", "perform pruning")
    ("elarnon,e", "perform elarnon's optimization")
    ("deadends,d", "take missed deadends if possible")
    ("untangle,u", "remove expensive cycles")
    ("backtrack,b", "remove some road at the ends of each car's path")
    ("search,s", "exhaustive search")
    ("swap,w", "prune+swap (ask elarnon)")
    ("depth", po::value<unsigned long>()->default_value(10), "search depth")
    ("no-stats", "don't display stats")
    ("balance,a", "banlance cars")
    ("final,l", "final optimization")
    ("kill,k", "kill a car")
    ("relax,r", "relax")
    ("force,f", "try all optimizations recursively (unsupported)")
    ;
  po::positional_options_description p;
  p.add("input", 1);
  p.add("output", 1);
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
	    options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }

  Routage routage;
  if (vm.count("map")) {
    FILE* f(fopen(vm["map"].as<string>().c_str(), "r"));
    routage.parse(f);
    fclose(f);
  } else {
    routage.parse(stdin);
  }
  routage.make_graph();
  if (vm.count("input")) {
    FILE* f(fopen(vm["input"].as<string>().c_str(), "r"));
    routage.parse_sol(f);
    fclose(f);
  }
  if (vm.count("prune")) {
    routage.prune(false);
  }
  if (vm.count("swap")) {
    for (int i(0); i < 70 + rand() % 100; ++i) {
      routage.swap();
    }
  }
  if (vm.count("final")) {
    routage.multi_elarnon();
  }
  if (vm.count("relax")) {
    routage.relax();
  }
  if (vm.count("correct")) {
    routage.correct();
  }
  if (vm.count("elarnon")) {
    routage.elarnon(vm["depth"].as<unsigned long>());
  }
  if (vm.count("deadends")) {
    routage.take_deadends();
  }
  if (vm.count("backtrack")) {
    routage.stat_last_prune();
  }
  if (vm.count("balance")) {
    routage.balance();
  }
  if (vm.count("kill")) {
    routage.kill();
  }
  if (vm.count("search")) {
    routage.search(vm["depth"].as<unsigned long>());
  }
  if (vm.count("untangle")) {
    routage.untangle();
  }
  if (vm.count("force")) {
    for (int i(0); i < 2; ++i) {
      routage.prune(false);
      routage.elarnon(vm["depth"].as<unsigned long>());
    }
  }
  if (!vm.count("no-stats")) {
    routage.do_stuff();
  }
  if (vm.count("output")) {
    FILE* f(fopen(vm["output"].as<string>().c_str(), "w"));
    routage.print(f);
    fclose(f);
  } else if (vm.count("input")) {
    string name = vm["input"].as<string>();
    if (vm.count("prune"))
      name += "p";
    if (vm.count("swap"))
      name += "w";
    if (vm.count("elarnon"))
      name += "e";
    if (vm.count("deadends"))
      name += "d";
    if (vm.count("backtrack"))
      name += "b";
    if (vm.count("untangle"))
      name += "u";
    if (vm.count("kill"))
      name += "k";
    if (vm.count("search"))
      name += "s";
    if (vm.count("correct"))
      name += "c";
    if (vm.count("force"))
      name += "f";
    if (vm.count("final"))
      name += "l";
    if (vm.count("balance"))
      name += "a";
    if (name != vm["input"].as<string>()) {
      FILE* f(fopen(name.c_str(), "w"));
      routage.print(f);
      fclose(f);
    }
  }
  return 0;
}
