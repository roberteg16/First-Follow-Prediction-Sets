#ifndef __PREDICTIONSET_H__
#define __PREDICTIONSET_H__

#include "Common.h"
#include "First.h"
#include "Following.h"
#include "Rules.h"

class PredictionSetGenerator {
public:
  PredictionSetGenerator(Rules &rules, Result &first, Result &following)
      : rules(rules), first(first), following(following) {
    // Init data
    init();
    process_production_set();
  }

  PredictionSetGenerator(string_view file) {
    RuleGenerator ruleGenerator(file);
    FirstGenerator firstGenerator(ruleGenerator.get_rules());
    FollowingGenerator followingGenerator(ruleGenerator.get_rules(),
                                          firstGenerator.get_first());
    rules = ruleGenerator.get_rules();
    first = firstGenerator.get_first();
    following = followingGenerator.get_following();

    init();
    process_production_set();
  }

  void print_rules();
  void print_first();
  void print_following();
  void print_prediction_set();

  void write_rules_to_file(string_view file, Rules &);

  Rules &get_rules() { return rules; }
  Result &get_first() { return first; }
  Result &get_following() { return following; }
  ProductionSet &get_production_set() { return production_set; }

  template <class T> void writeValue(T value, ostream &f) const {
    f.write(reinterpret_cast<char *>(&value), sizeof(value));
  }
  void writeString(const string &s, ostream &f) const;

  template <typename T = ostream> void print_production_set(T &out = cout) {
    for (auto e : production_set) {
      out << e.first << '\n';
      for (auto &v : e.second) {
        out << '\t';
        for (auto &ve : v)
          out << ve << ' ';
        out << '\n';
      }
      out << '\n';
    }
  }

private:
  void init();
  void process_production_set();
  set<string> first_of_production(Production &);

  Rules rules;
  Result first;
  Result following;

  // Result of PredictionSet
  ProductionSet production_set;
};
#endif
