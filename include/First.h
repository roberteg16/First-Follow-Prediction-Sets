#ifndef __FIRST_H__
#define __FIRST_H__

#include <Common.h>
#include <Rules.h>
#include <string_view>
#include <unordered_map>
#include <set>

namespace ffps {

using FirstSet = std::unordered_map<std::string, std::set<std::string>>;

/// Builds the FirstSet from a given set of Rules.
std::optional<FirstSet> BuildFirstSet(const Rules &rules);

void Print(const FirstSet &firstSet, std::ostream &out);

}; // namespace ffps

#endif // __FIRST_H__
