#include "Following.h"

void FollowingGenerator::init(Init_data_following &init_data,
                              set<string> &following_done) {
  // Initialize following and init_data with empty values
  for (auto &rule : rules) {
    following.insert(make_pair(rule.first, set<string>()));
    init_data.insert(
        make_pair(rule.first, vector<pair<string, vector<string>>>()));
  }

  // Insert $ to the first symbol of the grammar
  following.find(START)->second.insert("$");
  following_done.insert(START);

  // Fill init_data
  for (auto &rule : rules) {
    for (auto &v : rule.second) {
      for (int i = 0; i < v.size(); ++i) {
        if (is_nonterminal(rules, v[i])) {
          pair<string, vector<string>> pair;
          pair.first = rule.first;

          int size = (v.size() - i) - 1;
          if (size == 0) {
            pair.second.push_back(EPSILON);
          } else {
            for (int j = i + 1; j < v.size(); ++j)
              pair.second.push_back(v[j]);
          }
          init_data.find(v[i])->second.push_back(pair);
        }
      }
    }
  }

  // If there is a rule that does not have following, mark it as done
  for (auto &e : init_data) {
    if (e.second.size() == 0) {
      following_done.insert(e.first);
    }
  }
}

// Check if a rule is done
bool FollowingGenerator::is_rule_done(const string &rule,
                                      set<string> &following_d) {
  return following_d.find(rule) != following_d.end();
}

set<string> FollowingGenerator::first_of_production(
    string &rule_symbol, Production &prod, Result &first, bool &done,
    const string &current_rule_being_checked, set<string> &following_done) {
  set<string> set_to_return;
  int i = 0;
  // Go over all the symbols of the production
  while (i < prod.size() && !done) {
    auto &symbol = prod[i];

    // Nonterminal
    if (auto it = first.find(symbol); it != first.end()) {

      auto &set = it->second;
      // Insert all first of symbol
      set_to_return.insert(set.begin(), set.end());

      if (auto ep = set_to_return.find(EPSILON); ep != set_to_return.end()) {
        // Does contain EPSILON
        // Delete EPSILON
        set_to_return.erase(ep);
      } else {
        // Does not contain EPSILON
        // If it does not contain EPSILON then, we are done with first
        done = true;
        break;
      }
    }
    // Terminal
    else {
      if (symbol == EPSILON) {
        // do nothing and keep iteration
      } else {
        // If it is not EPSILON then, we are done with first
        done = true;
        set_to_return.insert(symbol);
        break;
      }
    }

    i++;
    // If we arrive at the end of the production that means that we have to
    // add the following of the rule of the production we are processing
    if (i == prod.size() && !done) {
      // Check if the following if the rule we are doing the production is done
      if (auto it = following_done.find(rule_symbol);
          it != following_done.end()) {
        // Following of the rule is done so we insert the following and mark it
        // as done
        done = true;
      }
      auto it2 = following.find(rule_symbol);
      set_to_return.insert(it2->second.begin(), it2->second.end());
    }
  }
  return set_to_return;
}

void FollowingGenerator::get_following(Init_data_following &init_data,
                                       set<string> &following_done) {
  int total_rules_to_be_done = rules.size();
  int rules_done = following_done.size();
  int prev = -1;
  int iterations = 0;
  // Iterate till all the rules are done
  while (rules_done < total_rules_to_be_done && iterations != MAX_ITERATIONS) {
    prev = rules_done;

    // Iterate over all rules
    for (auto &element : init_data) {
      // Check if the rule is done to avoid reprocess it
      if (!is_rule_done(element.first, following_done)) {
        bool done_rule = true;
        // Iterate over all the productions
        for (auto &follow : element.second) {
          bool done = false;
          // Get the followings
          auto set = first_of_production(follow.first, follow.second, first,
                                         done, element.first, following_done);
          // Check if done rule
          done_rule &= done;
          // Insert done production
          following.find(element.first)->second.insert(set.begin(), set.end());
        }

        // If all productions are done, then mark the rule as done
        if (done_rule)
          following_done.insert(element.first);
      }
    }

    // After a iteration get the new followings done
    rules_done = following_done.size();

    iterations = prev == rules_done ? iterations + 1 : 0;
  }
}
