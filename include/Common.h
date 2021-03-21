#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ffps {

constexpr std::string_view EpsilonStr{"epsilon"};

using Production = std::vector<std::string>;
using Productions = std::vector<Production>;
using Rules = std::unordered_map<std::string, Productions>;

bool IsNonTerminal(const Rules &rules, const std::string &symbol);
bool IsTerminal(const Rules &rules, const std::string &symbol);

} // namespace ffps

#endif // __COMMON_H__
