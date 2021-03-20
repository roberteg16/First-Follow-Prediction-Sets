#include <Common.h>

bool IsNonTerminal(const Rules &rules, const std::string &symbol) {
  return rules.find(symbol) != rules.end();
}

bool IsTerminal(const Rules &rules, const std::string &symbol) {
  return rules.find(symbol) == rules.end();
}
