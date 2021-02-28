#ifndef __FOLLOWING_H__
#define __FOLLOWING_H__

#include <Common.h>

class FollowingGenerator
{
  public:
    FollowingGenerator();
    FollowingGenerator(Rules& rules, Result& first) : rules(rules), first(first)
    {
      // Init data contains:
      // Given a rule, its list of following symbols of each production
      // The list it's compossed of the rule of the production and then the list of the symbols
      // that remain at the right side of the rule we are getting the following.

      // Rule_A
      //      -> Rule_of_the_production [list_of_symbols_at_the_right_side]
      //      -> Rule_of_the_production [list_of_symbols_at_the_right_side]
      InitDataFollowing init_data;

      // Following done
      std::set<std::string> following_done;

      // Init data
      init(init_data, following_done);
      get_following(init_data, following_done);
    }

    Result& get_following() { return following; }

    template <typename T = std::ostream>
    static void print_following(const Result &following, T &out = std::cout) {
      for(auto e : following)
      {
        out << e.first << '\n';
        out << '\t';
        for(auto& v : e.second)
        {
            out << v << ' ';
        }
        out << '\n';
      }
    }

  private:
    void init(InitDataFollowing&, std::set<std::string>&);
    void get_following(InitDataFollowing&, std::set<std::string>&);
    bool is_rule_done(const std::string& rule, std::set<std::string>& following_d);
    std::set<std::string> first_of_production(std::string&, Production&, Result&, bool&, std::set<std::string>&);

    Rules rules;
    Result first;

    //Result of following
    Result following;
};
#endif
