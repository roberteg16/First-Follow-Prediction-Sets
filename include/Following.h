#ifndef __FOLLOWING_H__
#define __FOLLOWING_H__

#include <Common.h>
#include <First.h>
#include <Rules.h>

namespace ffps {

using FollowingSet = std::unordered_map<std::string, std::set<std::string>>;

constexpr std::string_view StartStr{"Start"};

/// Builds the Following from a given set of Rules
std::optional<FollowingSet> BuildFollowingSet(const Rules &rules);

/// Builds the FirstSet from a given set of Rules, and the set of Firsts
std::optional<FollowingSet> BuildFollowingSet(const Rules &rules,
                                              const FirstSet &firstSet);

void Print(const FollowingSet &firstSet, std::ostream &out);

} // namespace ffps

#endif
