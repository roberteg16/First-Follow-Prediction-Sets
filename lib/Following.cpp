#include <Following.h>

#include "CicleDetector.h"
#include "StatusOfRule.h"

#include <algorithm>
#include <cassert>

using namespace ffps;

namespace {
struct ProductionStatus {
  /// Tracks whether the prod is done
  bool IsDone;
  /// Expansion of symbols for this production
  std::set<std::string_view> Expansion;
  /// The following symbol that is to be expanded
  std::string_view Following;

  explicit ProductionStatus(std::string_view following, bool done)
      : IsDone(done), Expansion(), Following(following) {}
  explicit ProductionStatus(std::set<std::string_view> expansions,
                            std::string_view following, bool done)
      : IsDone(done), Expansion(expansions), Following(following) {}
};

} // namespace

static bool
AreAllFollowingDone(const StatusOfRules<ProductionStatus> &statusOfRules) {
  return std::ranges::all_of(statusOfRules,
                             [](auto &pair) { return pair.second.Done; });
}

static bool TryResolveProd(StatusOfRules<ProductionStatus> &rulesStatus,
                           ProductionStatus &prodStatus) {

  RuleStatus<ProductionStatus> &ruleStatus = rulesStatus[prodStatus.Following];
  if (!ruleStatus.Done) {
    return false;
  }

  // Insert all symbols of following
  for (ProductionStatus &prods : ruleStatus.ProductionsStatus) {
    prodStatus.Expansion.insert(prods.Expansion.begin(), prods.Expansion.end());
  }

  return true;
}

/// Returns true when it accomplished to resolve the whole Rule
static bool TryResolveRule(std::vector<ProductionStatus> &statusByProd,
                           StatusOfRules<ProductionStatus> &rulesStatus) {
  bool allProdDone = true;
  for (ProductionStatus &prodStatus : statusByProd) {
    // Production done, skip
    if (prodStatus.IsDone) {
      continue;
    }

    prodStatus.IsDone = TryResolveProd(rulesStatus, prodStatus);

    // Accumulate done productions
    allProdDone &= prodStatus.IsDone;
  }

  return allProdDone;
}

static std::set<std::string_view>
RecoverFirstOfSubProduction(const Rules &rules, const FirstSet &firstSet,
                            const Production &prod, std::size_t &pos) {
  std::set<std::string_view> result;

  if (pos == prod.size()) {
    return {};
  }

  do {
    result.erase(EpsilonStr);

    const std::string &symbol = prod[pos++];
    if (IsTerminal(rules, symbol)) {
      result.insert(symbol);
    } else {
      // Not terminal.
      // Recover first set of the symbol
      auto firstsIt = firstSet.find(symbol);
      assert(firstsIt != firstSet.end());

      // Local firstSetCopy of it in case we need to delete epsilon
      const std::set<std::string> &firstSet = firstsIt->second;
      for (std::string_view sym : firstSet) {
        result.insert(sym);
      }
    }
  } while (result.contains(EpsilonStr) && pos < prod.size());

  return result;
}

static StatusOfRules<ProductionStatus>
InitializeStatusTrackerOfRules(const Rules &rules, const FirstSet &firstSet) {
  StatusOfRules<ProductionStatus> initData;

  // Start symbol is always '$'. Mark it as done from the beginning
  RuleStatus<ProductionStatus> &startRule = initData[StartStr];
  startRule.ProductionsStatus.emplace_back(std::set<std::string_view>{"$"},
                                           /*followingSymbol*/ "",
                                           /*done*/ true);

  // Iterate over all rules. Perform cross product to check whether a symbol
  // may appear or not in the rest of rules
  for (auto &[symbol, _] : rules) {
    for (auto &[symbolOfRuleOfCurrentProd, prods] : rules) {
      for (const Production &prod : prods) {

        // Check the curren offset from the end of the symbol we are looking
        // for to build the following
        auto it = std::ranges::find(prod, symbol);
        // Not symbol not in this production, continue
        if (it == prod.end()) {
          continue;
        }

        std::size_t pos = std::distance(prod.begin(), it);
        std::set<std::string_view> firstsOfProd =
            RecoverFirstOfSubProduction(rules, firstSet, prod, ++pos);

        if (firstsOfProd.contains(EpsilonStr.data()) || pos == prod.size()) {
          // It contains epsilon, that means that while getting firsts, we ended
          // at the last symbol. Remove it, add the symbols and mark to get
          // the following set of the rule of the current production where
          // we appeared
          firstsOfProd.erase(EpsilonStr);
          initData[symbol].ProductionsStatus.emplace_back(
              std::move(firstsOfProd), symbolOfRuleOfCurrentProd,
              /*done*/ false);
        } else {
          // Not epsilon, we did not reach the end, add the symbols and mark the
          // prod as done
          initData[symbol].ProductionsStatus.emplace_back(
              std::move(firstsOfProd), /*followingSymbol*/ "", /*done*/ true);
        }
      }
    }
  }

  return initData;
}

static FollowingSet
BuildFollowingSet(const StatusOfRules<ProductionStatus> &statusOfRules) {
  FollowingSet result;

  for (auto &[symbol, ruleStatus] : statusOfRules) {
    assert(ruleStatus.Done && "Rule not done!");
    for (const ProductionStatus &prodStatus : ruleStatus.ProductionsStatus) {
      assert(prodStatus.IsDone && "Production not done!");
      for (std::string_view symbolFirstProd : prodStatus.Expansion) {
        result[symbol.data()].emplace(symbolFirstProd);
      }
    }
  }

  return result;
}

static bool RulesContainsStartSymbol(const Rules &rules) {
  return rules.contains(StartStr.data());
}

std::optional<FollowingSet> ffps::BuildFollowingSet(const Rules &rules,
                                                    const FirstSet &firstSet) {

  if (!RulesContainsStartSymbol(rules)) {
    std::cerr << "The grammar does not contain a 'Start' symbol. Not able to "
                 "generate the following set.\n";
    return {};
  }

  // Initialize first tracking data
  StatusOfRules<ProductionStatus> rulesStatus =
      InitializeStatusTrackerOfRules(rules, firstSet);

  while (!AreAllFollowingDone(rulesStatus)) {
    /// If a cycle is formed, stop the process
    if (FirstSetOfAnotherFirstSetOfUsCycle(rulesStatus) ||
        FirstSetOfItselfCycle(rulesStatus)) {
      return {};
    }

    for (auto &[symbol, ruleStatus] : rulesStatus) {
      // Rule done, skip
      if (ruleStatus.Done) {
        continue;
      }

      // Try resolve our current rule
      ruleStatus.Done =
          TryResolveRule(ruleStatus.ProductionsStatus, rulesStatus);
    } /// Builds the FirstSet from a given set of Rules.
    std::optional<FollowingSet> BuildFollowingSet(const Rules &rules,
                                                  const FirstSet &firstSet);
  }

  return ::BuildFollowingSet(rulesStatus);
}

std::optional<FollowingSet> BuildFollowingSet(const Rules &rules) {
  std::optional<ffps::FirstSet> firstSet = ffps::BuildFirstSet(rules);
  if (!firstSet) {
    return {};
  }

  return BuildFollowingSet(rules, *firstSet);
}
