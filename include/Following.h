#ifndef __FOLLOWING_H__
#define __FOLLOWING_H__

#include "Common.h"

class FollowingGenerator {
public:
  FollowingGenerator();
  FollowingGenerator(Rules &rules, Result &first) : rules(rules), first(first) {
    // Init data contains:
    // Given a rule, its list of following symbols of each production
    // The list it's compossed of the rule of the production and then the list
    // of the symbols that remain at the right side of the rule we are getting
    // the following.

    // Rule_A
    //      -> Rule_of_the_production [list_of_symbols_at_the_right_side]
    //      -> Rule_of_the_production [list_of_symbols_at_the_right_side]
    Init_data_following init_data;

    // Following done
    set<string> following_done;

    // Init data
    init(init_data, following_done);
    get_following(init_data, following_done);
  }

  Result &get_following() { return following; }

  template <typename T = ostream>
  static void print_following(Result &following, T &out = cout) {
    for (auto e : following) {
      out << e.first << '\n';
      out << '\t';
      for (auto &v : e.second) {
        out << v << ' ';
      }
      out << '\n';
    }
  }

private:
  void init(Init_data_following &, set<string> &);
  void get_following(Init_data_following &, set<string> &);
  bool is_rule_done(const string &rule, set<string> &following_d);
  set<string> first_of_production(string &, Production &, Result &, bool &,
                                  const string &, set<string> &);

  Rules rules;
  Result first;

  // Result of following
  Result following;
};
#endif
