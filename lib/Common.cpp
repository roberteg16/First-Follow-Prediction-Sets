#include "Common.h"

bool is_nonterminal(const Rules &rules, const string &symbol) {
  return rules.find(symbol) != rules.end();
}

bool is_terminal(const Rules &rules, const string &symbol) {
  return !is_nonterminal(rules, symbol);
}
