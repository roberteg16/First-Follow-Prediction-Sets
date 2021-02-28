#include <PredictionSet.h>

void PredictionSetGenerator::init() {
  for (auto &rule : Rules) {
    std::pair<std::string, Productions> pair_ =
        make_pair(rule.first, Productions());
    production_set.insert(pair_);
  }
}

std::set<std::string>
PredictionSetGenerator::first_of_production(const Production &production) {
  std::set<std::string> return_value;
  for (std::size_t i = 0; i < production.size(); ++i) {
    auto &symbol = production[i];
    if (auto it = First.find(symbol); it != First.end()) {
      // Non terminal
      auto &set = it->second;
      return_value.insert(set.begin(), set.end());

      if (auto it2 = return_value.find(EpsilonStr.data());
          it2 != return_value.end() && i != production.size() - 1) {
        // Does contain epsilon
        return_value.erase(it2);
      } else {
        // Does not contain EpsilonStr.data()
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

  for (auto &rule : Rules) {
    for (auto &production : rule.second) {

      auto set_ = first_of_production(production);

      std::set<std::string> avoid_duplicates;

      if (auto it = set_.find(EpsilonStr.data()); it != set_.end()) {
        // Does contain EpsilonStr.data(), so we delete it and insert following
        set_.erase(it);
        auto &following_set = Following.find(rule.first)->second;

        avoid_duplicates.insert(following_set.begin(), following_set.end());
      }
      avoid_duplicates.insert(set_.begin(), set_.end());

      Production v(avoid_duplicates.begin(), avoid_duplicates.end());
      production_set.find(rule.first)->second.push_back(move(v));
    }
  }
}

void PredictionSetGenerator::writeString(const std::string &s,
                                         std::ostream &f) const {
  f.write(s.c_str(), s.size());
  f.write("\0", sizeof(char));
}

void PredictionSetGenerator::write_rules_to_file(
    std::string_view file,
    const std::unordered_map<std::string, Productions> &r) {
  std::ofstream outfile(file.data(), std::ios::binary | std::ios::out);

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

void PredictionSetGenerator::print_rules() { ffps::Print(Rules, std::cout); }

void PredictionSetGenerator::print_first() {
  FirstGenerator::print_first(First);
}

void PredictionSetGenerator::print_following() {
  FollowingGenerator::print_following(Following);
}

void PredictionSetGenerator::print_prediction_set() {
  ffps::Print(production_set, std::cout);
}
