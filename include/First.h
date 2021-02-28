#ifndef __FIRST_H__
#define __FIRST_H__

#include "Common.h"

class FirstGenerator {
public:
  FirstGenerator();
  FirstGenerator(Rules &rules) : rules(rules) {
    // Mid data that we will be appeding to new symbols
    Rules mid_rules;
    // Index of the position of the symbol we are processing for production of
    // each rule
    unordered_map<string, vector<int>> index_of_rules;
    // Rules done
    set<string> rule_done;
    // Productions done
    unordered_map<string, vector<bool>> production_done;

    init(mid_rules, index_of_rules, production_done);
    get_first(mid_rules, rule_done, production_done, index_of_rules);
  }

  Result &get_first() { return first; }

  template <typename T = ostream>
  static void print_first(Result &first, T &out = cout) {
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
  void get_first(Rules &, set<string> &, unordered_map<string, vector<bool>> &,
                 unordered_map<string, vector<int>> &);
  void init(Rules &, unordered_map<string, vector<int>> &,
            unordered_map<string, vector<bool>> &);
  bool is_rule_done(const string &, set<string> &);
  bool is_production_done(const string &, int,
                          unordered_map<string, vector<bool>> &);
  set<string> first_of_symbol(const string &, int, const string &, bool &,
                              Rules &, set<string> &,
                              unordered_map<string, vector<int>> &);

  Rules rules;
  // End data. The result of the algorithm, all the first symbols of each non
  // terminal.
  Result first;
};
#endif
