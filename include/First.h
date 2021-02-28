#ifndef __FIRST_H__
#define __FIRST_H__

#include <Common.h>

class FirstGenerator {
public:
  FirstGenerator();
  FirstGenerator(const Rules &rules) : rules(rules) {
    // Mid data that we will be appeding to new symbols
    Rules mid_rules;
    // Index of the position of the symbol we are processing for production of
    // each rule
    std::unordered_map<std::string, std::vector<int>> index_of_rules;
    // Rules done
    std::set<std::string> rule_done;
    // Productions done
    std::unordered_map<std::string, std::vector<bool>> production_done;

    init(mid_rules, index_of_rules, production_done);
    get_first(mid_rules, rule_done, production_done, index_of_rules);
  }

  Result &get_first() { return first; }

  template <typename T = std::ostream>
  static void print_first(const Result &first, T &out = std::cout) {
    for (auto e : first) {
      out << e.first << '\n';
      out << '\t';
      for (auto &v : e.second) {
        out << v << ' ';
      }
      out << '\n';
    }
  }

private:
  void get_first(Rules &, std::set<std::string> &,
                 std::unordered_map<std::string, std::vector<bool>> &,
                 std::unordered_map<std::string, std::vector<int>> &);
  void init(Rules &, std::unordered_map<std::string, std::vector<int>> &,
            std::unordered_map<std::string, std::vector<bool>> &);
  bool is_rule_done(const std::string &, std::set<std::string> &);
  bool is_production_done(const std::string &, int,
                          std::unordered_map<std::string, std::vector<bool>> &);
  std::set<std::string>
  first_of_symbol(const std::string &, int, const std::string &, bool &,
                  Rules &, std::set<std::string> &,
                  std::unordered_map<std::string, std::vector<int>> &);

  const Rules &rules;
  // End data. The result of the algorithm, all the first symbols of each non
  // terminal.
  Result first;
};
#endif
