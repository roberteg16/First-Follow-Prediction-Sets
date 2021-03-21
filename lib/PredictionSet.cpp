#include <PredictionSet.h>

#include <cassert>
#include <string_view>

using namespace ffps;

static std::set<std::string_view>
GetUnionOfFirstsOfProd(const Rules &rules, const FirstSet &firstSet,
                       const Production &prod) {
  std::set<std::string_view> result;

  for (std::string_view sym : prod) {
    if (IsTerminal(rules, sym.data())) {
      result.insert(sym);
    } else {
      auto itFirstSet = firstSet.find(sym.data());
      assert(itFirstSet != firstSet.end() && "Symbol not in FirstSet!");

      result.insert(itFirstSet->second.begin(), itFirstSet->second.end());
    }
  }

  return result;
}

ffps::PredictionSet
ffps::BuildPredictionSet(const Rules &rules, const FirstSet &firstSet,
                         const FollowingSet &followingSet) {
  ffps::PredictionSet predSet;

  for (auto &[symbol, prods] : rules) {
    for (const Production &prod : prods) {
      std::set<std::string_view> unionOfFirsts =
          GetUnionOfFirstsOfProd(rules, firstSet, prod);

      if (unionOfFirsts.contains(EpsilonStr)) {
        // The union of the firsts, contain epsilon, add following of the rule
        // and erase epsilon afterwards

        // Insert Following(symbol)
        auto itFollowingSet = followingSet.find(symbol);
        assert(itFollowingSet != followingSet.end() &&
               "Symbol not in FollowingSet!");

        unionOfFirsts.insert(itFollowingSet->second.begin(),
                             itFollowingSet->second.end());
        // Erase epsilon
        unionOfFirsts.erase(EpsilonStr);
      }

      // Copy result data
      std::vector<std::string> prodResult;
      prodResult.reserve(unionOfFirsts.size());
      for (std::string_view sym : unionOfFirsts) {
        prodResult.emplace_back(sym);
      }
      predSet[symbol].push_back(std::move(prodResult));
    }
  }

  return predSet;
}

std::optional<ffps::PredictionSet>
ffps::BuildPredictionSet(const Rules &rules) {
  std::optional<ffps::FirstSet> firstSet = ffps::BuildFirstSet(rules);
  if (!firstSet) {
    return {};
  }

  std::optional<ffps::FollowingSet> followingSet =
      ffps::BuildFollowingSet(rules, *firstSet);
  if (!firstSet) {
    return {};
  }

  return BuildPredictionSet(rules, *firstSet, *followingSet);
}