#include <Common.h>

using namespace ffps;

bool ffps::IsNonTerminal(const Rules &rules, const std::string &symbol) {
  return rules.find(symbol) != rules.end();
}

bool ffps::IsTerminal(const Rules &rules, const std::string &symbol) {
  return rules.find(symbol) == rules.end();
}
