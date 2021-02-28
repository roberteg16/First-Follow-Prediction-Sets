#include "PredictionSet.h"

void PredictionSetGenerator::init() {
  for (auto &rule : rules) {
    pair<string, Productions> pair_ = make_pair(rule.first, Productions());
    production_set.insert(pair_);
  }
}

set<string>
PredictionSetGenerator::first_of_production(Production &production) {
  set<string> return_value;
  for (int i = 0; i < production.size(); ++i) {
    auto &symbol = production[i];
    if (auto it = first.find(symbol); it != first.end()) {
      // Non terminal
      auto &set = it->second;
      return_value.insert(set.begin(), set.end());

      if (auto it2 = return_value.find(EPSILON);
          it2 != return_value.end() && i != production.size() - 1) {
        // Does contain epsilon
        return_value.erase(it2);
      } else {
        // Does not contain EPSILON
        break;
      }
    } else {
      // Terminal
      return_value.insert(symbol);
      break;
    }
  }

  return return_value;
}

void PredictionSetGenerator::process_production_set() {
  init();

  for (auto &rule : rules) {
    for (auto &production : rule.second) {

      auto set_ = first_of_production(production);

      set<string> avoid_duplicates;

      if (auto it = set_.find(EPSILON); it != set_.end()) {
        // Does contain EPSILON, so we delete it and insert following
        set_.erase(it);
        auto &following_set = following.find(rule.first)->second;

        avoid_duplicates.insert(following_set.begin(), following_set.end());
      }
      avoid_duplicates.insert(set_.begin(), set_.end());

      Production v(avoid_duplicates.begin(), avoid_duplicates.end());
      production_set.find(rule.first)->second.push_back(move(v));
    }
  }
}

void PredictionSetGenerator::writeString(const string &s, ostream &f) const {
  f.write(s.c_str(), s.size());
  f.write("\0", sizeof(char));
}

void PredictionSetGenerator::write_rules_to_file(string_view file, Rules &r) {
  ofstream outfile(file.data(), ios::binary | ios::out);

  if (outfile.is_open()) {
    writeValue(r.size(), outfile);
    for (auto &e : r) {
      writeString(e.first, outfile);
      writeValue(e.second.size(), outfile);
      for (auto &v : e.second) {
        writeValue(v.size(), outfile);
        for (auto &str : v) {
          writeString(str, outfile);
        }
      }
    }
  }
}

void PredictionSetGenerator::print_rules() {
  RuleGenerator::print_rules(rules);
}

void PredictionSetGenerator::print_first() {
  FirstGenerator::print_first(first);
}

void PredictionSetGenerator::print_following() {
  FollowingGenerator::print_following(following);
}

void PredictionSetGenerator::print_prediction_set() {
  RuleGenerator::print_rules(production_set);
}
