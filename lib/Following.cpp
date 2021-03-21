#include <Following.h>

#include "CicleDetector.h"
#include "StatusOfRule.h"
#include <cassert>

using namespace ffps;

namespace {
struct ProductionStatus {
  /// Tracks whether the prod is done
  bool Done;
  /// Expansion of symbols for this production
  std::set<std::string_view> Expansion;
  /// The following symbol that is to be expanded
  std::string_view Following;

  explicit ProductionStatus(std::string_view following)
      : Done(false), Expansion(), Following(following) {}
  explicit ProductionStatus(std::set<std::string_view> syms,
                            std::string_view following)
      : Done(false), Expansion(syms), Following(following) {}
};

} // namespace

static bool
AreAllFollowingDone(const StatusOfRules<ProductionStatus> &statusOfRules) {
  for (auto &[symbol, ruleStatus] : statusOfRules) {
    if (!ruleStatus.Done) {
      return false;
    }
  }

  return true;
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
    if (prodStatus.Done) {
      continue;
    }

    prodStatus.Done = TryResolveProd(rulesStatus, prodStatus);

    // Accumulate done productions
    allProdDone &= prodStatus.Done;
  }

  return allProdDone;
}

static StatusOfRules<ProductionStatus>
InitializeStatusTrackerOfRules(const Rules &rules, const FirstSet &firstSet) {
  StatusOfRules<ProductionStatus> initData;

  // Start symbol is always '$'. Mark it as done from the beginning
  RuleStatus<ProductionStatus> &startRule = initData[StartStr];
  std::set<std::string_view> set{"$"};
  startRule.ProductionsStatus.emplace_back(set, "");
  startRule.ProductionsStatus.back().Done = true;

  // Iterate over all rules. Perform cross product to check whether a symbol
  // may appear or not in the rest of rules
  for (auto &[symbol, _] : rules) {
    for (auto &[symbolOfRuleOfCurrentProd, prods] : rules) {
      for (const Production &prod : prods) {

        // Check the curren offset from the end of the symbol we are looking
        // for to build the following
        std::size_t offsetFromEnd =
            std::distance(std::ranges::find(prod, symbol), prod.end());

        // Not symbol not in this production, continue
        if (!offsetFromEnd) {
          continue;
        }

        // The symbol is the last one of the production
        if (offsetFromEnd == 1) {
          // Avoid getting following of ourself
          if (symbol == symbolOfRuleOfCurrentProd) {
            continue;
          }

          // Get the symbol of the rule of the current production has the
          // following
          initData[symbol].ProductionsStatus.emplace_back(
              symbolOfRuleOfCurrentProd);
          continue;
        }

        // Symbol is not the last one of the production, recover the next one.
        // We'll recover the following symbols from it
        auto &followSymbol = prod[(prod.size() - offsetFromEnd) + 1];

        // If it is terminal, add the symbol and mark the prod as done
        if (IsTerminal(rules, followSymbol)) {
          ProductionStatus &prodRule =
              initData[symbol].ProductionsStatus.emplace_back(
                  std::set<std::string_view>{followSymbol}, "");
          prodRule.Done = true;
          continue;
        }

        // Not terminal.
        // Recover first set of the symbol
        auto firstsIt = firstSet.find(followSymbol);
        assert(firstsIt != firstSet.end());

        // Local copy of it in case we need to delete epsilon
        const std::set<std::string> &firstSet = firstsIt->second;
        std::set<std::string_view> copy;
        for (std::string_view sym : firstSet) {
          copy.insert(sym);
        }

        if (firstSet.contains(EpsilonStr.data())) {
          // It contains epsilon, remove it, add the symbols and mark to get
          // the following set of the rule of the current production where
          // we appeared
          copy.erase(EpsilonStr);
          initData[symbol].ProductionsStatus.emplace_back(
              copy, symbolOfRuleOfCurrentProd);
        } else {
          // Not epsilon, add the symbols and mark the prod as done
          ProductionStatus &prodRule =
              initData[symbol].ProductionsStatus.emplace_back(copy, "");
          prodRule.Done = true;
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
      assert(prodStatus.Done && "Production not done!");
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
    }
  }

  return ::BuildFollowingSet(rulesStatus);
}
