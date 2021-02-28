#ifndef __RULES_H__
#define __RULES_H__

#include "Common.h"
#include <algorithm>
#include <climits>

#define END_OF_CLASES "===="

struct Variable {
  string type;
  string name;
};

class RuleGenerator {
public:
  RuleGenerator(string_view file) {
    load_rules(file);

    /*for(auto& class_ : classes)
    {
      cout << class_.first << endl;
      for(auto& var : class_.second)
      {
        cout << "\t" << var.type << " " << var.name << endl;
      }
    }*/

    solve_left_recursion();
    while (solve_common_factors_by_the_left())
      ;
  }

  void load_rules(string_view);

  template <typename T = ostream>
  static void print_rules(Rules &rules, T &out = cout) {
    for (auto e : rules) {
      out << e.first << ENDL;
      for (auto &v : e.second) {
        out << '\t';
        for (auto &ve : v)
          cout << ve << ' ';
        out << ENDL;
      }
      out << ENDL;
    }
  }

  Rules &get_rules() { return rules; }

private:
  void trim(string &);
  void type_and_var(string &, string &, string &);
  vector<string> split(const string &, const char);
  vector<string> load_lines(const char *);
  void solve_left_recursion();
  bool solve_common_factors_by_the_left();
  bool exists_in_productions(const string &, const vector<string> &);
  void insert_in_rules(const string &, vector<string> &&);

  Rules rules;
  unordered_map<string, vector<Variable>> classes;
};

#endif
