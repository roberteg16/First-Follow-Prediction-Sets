#include "Rules.h"

void RuleGenerator::type_and_var(string &line, string &type, string &var) {
  char separator = ' ';
  size_t pos = line.find(separator);
  size_t initialPos = 0;

  type = line.substr(initialPos, pos - initialPos);
  initialPos = pos + 1;
  pos = line.find(separator, initialPos);
  var = line.substr(initialPos, pos - initialPos);
}

vector<string> RuleGenerator::split(const string &txt, const char ch) {
  size_t pos = txt.find(ch);
  size_t initialPos = 0;

  vector<string> strs;
  // Decompose statement
  while (pos != string::npos) {
    strs.push_back(txt.substr(initialPos, pos - initialPos));

    initialPos = pos + 1;
    pos = txt.find(ch, initialPos);
  }

  if (initialPos < txt.size())
    strs.push_back(txt.substr(initialPos, txt.size() - initialPos));

  return strs;
}

void RuleGenerator::trim(string &line) {
  int start = 0;
  int end = line.size() - 1;

  while (line[start] == ' ' || line[start] == '\t')
    start++;
  while (line[end] == ' ' || line[end] == '\t')
    end--;
  line = line.substr(start, (end + 1) - start);
}

vector<string> RuleGenerator::load_lines(const char *file_name) {
  vector<string> rules_s;

  ifstream t(file_name);

  string line;
  while (std::getline(t, line)) {
    if (line.size() > 0) {
      trim(line);
      rules_s.push_back(move(line));
    }
  }
  t.close();

  return rules_s;
}

void RuleGenerator::insert_in_rules(const string &rule,
                                    vector<string> &&production) {
  auto it = rules.find(rule);

  if (it != rules.end()) {
    // Does exist
    it->second.push_back(move(production));
  } else {
    // Does not exist
    pair<string, vector<vector<string>>> p;
    p.first = rule;
    p.second.push_back(move(production));

    rules.insert(move(p));
  }
}

bool RuleGenerator::exists_in_productions(const string &rule,
                                          const vector<string> &production) {
  auto it = rules.find(rule);

  if (it == rules.end())
    return false;

  auto &productions = it->second;

  for (auto &production_ : productions) {
    bool exit = false;
    int i = 0;
    for (; i < production.size() && i < production_.size() && !exit; ++i) {
      if (production_[i] != production[i])
        exit = true;
    }
    if (i == production.size() && i == production_.size() && !exit)
      return true;
  }
  return false;
}

bool RuleGenerator::solve_common_factors_by_the_left() {
  for (auto rule_pair = rules.begin(); rule_pair != rules.end(); ++rule_pair) {
    vector<vector<string>> &productions = rule_pair->second;

    vector<set<int>> index_of_left_common_factors;
    for (int i = 0; i < productions.size(); ++i) {
      for (auto &set_of_index : index_of_left_common_factors)
        for (auto element : set_of_index)
          if (i == element)
            goto do_nothing;

      {
        set<int> index;
        for (int j = 0; j < productions.size(); ++j) {
          if (i != j && productions[i][0] == productions[j][0]) {
            index.insert(i);
            index.insert(j);
          }
        }
        if (index.size() > 0)
          index_of_left_common_factors.push_back(move(index));
      }

    do_nothing:;
    }

    if (index_of_left_common_factors.size() > 0) {
      vector<vector<string>> productions_to_add_after_deleted_old_ones;
      vector<vector<int>> indexs;
      for (auto &set_of_index : index_of_left_common_factors) {
        // Extract minimun index inside productions
        vector<int> index(set_of_index.begin(), set_of_index.end());
        sort(index.begin(), index.end());

        int minimun = INT_MAX;
        for (int i = 0; i < index.size(); ++i) {
          for (int j = i + 1; j < index.size(); ++j) {

            int k = 0;
            while (productions[index[i]][k] == productions[index[j]][k] &&
                   k < productions[index[i]].size() &&
                   k < productions[index[j]].size()) {
              k++;
            }
            minimun = min(minimun, k);
          }
        }

        // Got minimun
        string new_rule_str = rule_pair->first + "_CFL";

        for (int i = 0; i < index.size(); ++i) {
          int starter = minimun;
          vector<string> new_production;

          if (minimun == productions[index[i]].size()) {
            // Insert epsilon
            new_production.emplace_back("epsilon");
          } else {
            // Insert remaining symbols
            while (starter < productions[index[i]].size()) {
              new_production.push_back(productions[index[i]][starter]);
              starter++;
            }
          }

          if (!exists_in_productions(new_rule_str, new_production))
            insert_in_rules(new_rule_str, move(new_production));
        }

        vector<string> modified_production;
        for (int i = 0; i < minimun; ++i) {
          modified_production.push_back(productions[index[0]][i]);
        }
        modified_production.push_back(move(new_rule_str));
        productions_to_add_after_deleted_old_ones.push_back(
            move(modified_production));
        indexs.push_back(move(index));
      }

      vector<int> already_deleted;
      for (auto &index : indexs) {
        for (auto i : index) {
          int deleted = 0;
          for (auto element : already_deleted) {
            if (element < i)
              deleted++;
          }
          int aux_index = i - deleted;

          auto it = productions.begin();
          while (aux_index--)
            ++it;
          productions.erase(it);
          already_deleted.push_back(i);
        }
      }

      for (auto &e : productions_to_add_after_deleted_old_ones)
        productions.push_back(move(e));

      return true;
    }
  }
  return false;
}

void RuleGenerator::solve_left_recursion() {
  for (auto &rule : rules) {
    for (auto it = rule.second.begin(); it != rule.second.end(); ++it) {
      if (rule.second.size() > 0 && rule.first == (*it)[0]) {
        string new_rule(rule.first + "_LR");
        vector<string> right_symbols(++(*it).begin(), (*it).end());
        rule.second.erase(it);

        for (auto &rewrite_rule : rule.second) {
          rewrite_rule.push_back(new_rule);
        }

        // New Rule
        pair<string, vector<vector<string>>> p;
        p.first = new_rule;
        right_symbols.push_back(move(new_rule));
        p.second.push_back(move(right_symbols));
        vector<string> v;
        v.emplace_back("epsilon");
        p.second.push_back(move(v));

        rules.insert(move(p));

        break;
      }
    }
  }
}

void RuleGenerator::load_rules(string_view file_name) {
  vector<string> lines = load_lines(file_name.data());

  auto it = lines.begin();

  // Load Classes
  while (it != lines.end()) {

    if (*it == END_OF_CLASES) {
      ++it;
      break;
    }

    // Read Class
    vector<Variable> vec;
    string name_class = move(*it);
    ++it;

    // Read Type and name
    while (*it != ";") {
      struct Variable var;

      type_and_var(*it, var.type, var.name);
      vec.push_back(move(var));
      ++it;
    }
    ++it;
    classes.insert(make_pair(move(name_class), move(vec)));
  }

  // Load Rules
  while (it != lines.end()) {
    /*vector<string> v = split(*it, ' ');

    string rule = move(v[0]);

    bool has_especial_return_type = v[1] != "->";
    size_t starting_rules = has_especial_return_type ? 3 : 2;

    string especial_return_type;
    if(has_especial_return_type)
      especial_return_type = move(v[1]);

    vector< vector<string> > vector_aux;
    */

    // Get first
    vector<string> v = split(*it, ' ');
    string rule = move(v[0]);
    Productions all_productions;

    Production rules_v;
    for (int i = 2; i < v.size(); ++i)
      rules_v.push_back(move(v[i]));

    all_productions.push_back(move(rules_v));

    bool exit = false;
    // get all next productions of the same rule
    ++it;
    while (!exit && it != lines.end()) {
      rules_v.clear();
      v = split(*it, ' ');

      if (v[0] == "|") {
        for (int i = 1; i < v.size(); ++i)
          rules_v.push_back(move(v[i]));
        all_productions.push_back(move(rules_v));
        ++it;
      } else {
        exit = true;
      }
    }

    rules.insert(make_pair(move(rule), move(all_productions)));
  }
}
