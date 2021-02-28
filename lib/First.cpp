#include "Common.h"
#include <First.h>

int num_of_rules = 0;
int current_num_of_rules_done = 0;

void FirstGenerator::init(
    Rules &mid_rules, std::unordered_map<std::string, std::vector<int>> &index_of_rules,
    std::unordered_map<std::string, std::vector<bool>> &production_done) {
  for (auto &e : rules) {
    Productions p;

    for (auto &v : e.second)
      p.push_back({v[0]});

    mid_rules.insert(make_pair(e.first, p));
    first.insert(make_pair(e.first, std::set<std::string>()));
    production_done.insert(
        make_pair(e.first, std::vector<bool>(e.second.size(), false)));
    index_of_rules.insert(make_pair(e.first, std::vector<int>(1, 0)));
  }
  num_of_rules = rules.size();
}

bool FirstGenerator::is_rule_done(const std::string &rule, std::set<std::string> &rule_done) {
  return rule_done.find(rule) != rule_done.end();
}

bool FirstGenerator::is_production_done(
    const std::string &rule, int production_index,
    std::unordered_map<std::string, std::vector<bool>> &production_done) {
  return production_done.find(rule)->second[production_index];
}

std::set<std::string> FirstGenerator::first_of_symbol(
    const std::string &rule, int production_index, const std::string &symbol, bool &done,
    Rules &mid_rules, std::set<std::string> &rule_done,
    std::unordered_map<std::string, std::vector<int>> &index_of_rules) {
  // If it is terminal, mark production as done and retrun the symbol
  if (IsTerminal(rules, symbol)) {
    done = true;
    return {symbol};
  } else {
    // If it is not terminal check if the symbol we are processing has all his
    // productions done
    if (is_rule_done(symbol, rule_done)) {
      // Get his firsts
      std::set<std::string> production = first.find(symbol)->second;

      // Check if it has ε
      if (auto it = production.find(EpsilonStr.data()); it != production.end()) {
        // Does contain ε

        // As it contains ε, we need to process the first of the next symbol
        std::size_t next_symbol =
            ++index_of_rules.find(symbol)->second[production_index];
        if (next_symbol < rules.find(rule)->second[production_index].size()) {
          // Delete ε
          production.erase(it);
          // Add the next symbol to or mid representation of the production so
          // the next iteration will process the next new symbol of the
          // production
          mid_rules.find(rule)->second[production_index].push_back(
              rules.find(rule)->second[production_index][next_symbol]);
        } else {
          // If it is the last symbol of the production, we conserve ε and mark
          // the production has done
          done = true;
        }

        return production;
      } else {
        // If it does not contains ε, then we are done
        done = true;
      }

      return production;
    } else {
      // If the rule of the symbol we are getting is not done then we return
      // nothing do not mark the production as done, and wait to the next
      // iteration to resolve this rule
      return first.find(symbol)->second;
    }
  }
}

void FirstGenerator::get_first(
    Rules &mid_rules, std::set<std::string> &rule_done,
    std::unordered_map<std::string, std::vector<bool>> &production_done,
    std::unordered_map<std::string, std::vector<int>> &index_of_rules) {

  // While there is still one rule to be done, keep iterating
  for (int cont = 0; cont < 100; ++cont) {
    // Iterate over all rules
    for (const auto &rule : mid_rules) {
      // Only process the rules that are still waiting to be done
      if (!is_rule_done(rule.first, rule_done)) {
        // If all the productions are done, then the rule is done
        bool completed_rule = true;

        // Iterate over all productions of each rules
        for (std::size_t i = 0; i < rule.second.size(); ++i) {
          // Only process the remaining productions
          if (!is_production_done(rule.first, i, production_done)) {
            // Get next symbol of production to process
            std::string symbol(rule.second[i][rule.second[i].size() - 1]);
            bool done = false;
            std::set<std::string> first_of =
                first_of_symbol(rule.first, i, symbol, done, mid_rules,
                                rule_done, index_of_rules);

            // If all productions are done, then the rule is done
            completed_rule &= done;

            // If the production is done, the mark it as finished
            if (done)
              production_done.find(rule.first)->second[i] = true;

            // Append the news symbols of the production to the already
            // existings rules
            first.find(rule.first)
                ->second.insert(first_of.begin(), first_of.end());
          }
        }
        // If rule is completed, mark it as finish and increment the counter of
        // the rules already done
        if (completed_rule) {
          rule_done.insert(rule.first);
          current_num_of_rules_done++;
        }
      }
    }
  }
}
