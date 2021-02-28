#ifndef __COMMON_H__
#define __COMMON_H__

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

constexpr std::string_view StartStr{"Start"};
constexpr std::string_view EpsilonStr{"epsilon"};
#define MAX_ITERATIONS 100

using Result = std::unordered_map<std::string, std::set<std::string>>;
using Production = std::vector<std::string>;
using Productions = std::vector<Production>;

using PartialRule = struct {
  std::string Rule;
  Production Production;
};

using Rule = struct {
  std::string Rule;
  Productions Productions;
};

using Rules = std::unordered_map<std::string, Productions>;
using ProductionSet = Rules;
using InitDataFollowing = std::unordered_map<
    std::string, std::vector<std::pair<std::string, std::vector<std::string>>>>;

bool IsNonterminal(const Rules &rules, const std::string &symbol);
bool IsTerminal(const Rules &rules, const std::string &symbol);

#endif
