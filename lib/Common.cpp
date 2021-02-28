#include <Common.h>

bool IsNonterminal(const Rules& rules, const std::string& symbol)
{
  return rules.find(symbol) != rules.end();
}

bool IsTerminal(const Rules& rules, const std::string& symbol)
{
  return !IsNonterminal(rules, symbol);
}
