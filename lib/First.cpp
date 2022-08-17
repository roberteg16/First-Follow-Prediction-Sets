#include <First.h>

#include "CicleDetector.h"
#include "StatusOfRule.h"

#include <algorithm>
#include <cassert>

using namespace ffps;

namespace {
struct ProductionStatus {
  /// Tracks whether the prod is done
  bool IsDone;
  /// Original production
  const Production &Prod;
  /// Current index in the original production
  std::size_t CurrentIndex;
  /// Current expansion of symbols for this production
  std::set<std::string_view> Expansion;

  explicit ProductionStatus(const Production &prod, std::string_view sym)
      : IsDone(false), Prod(prod), CurrentIndex(1), Expansion({sym}) {}
};

} // namespace

static bool
AreAllFirstDone(const StatusOfRules<ProductionStatus> &statusOfRules) {
  return std::ranges::all_of(statusOfRules,
                             [](auto &pair) { return pair.second.Done; });
}

static bool AreAllTerminal(const Rules &rules, ProductionStatus &prodStatus) {
  return std::ranges::all_of(prodStatus.Expansion, [&](std::string_view sym) {
    return IsTerminal(rules, sym.data());
  });
}

/// Returns a bool indicating whether we added a new symbol to the production.
/// In case that there is a epsilon and there are still symbols to be added,
/// then we need more iterations, so we return true. True otherwise
static bool AddNextSymbolInProdIfContainsEpsilon(ProductionStatus &prodStatus) {
  // Check if it has epsilon
  if (prodStatus.Expansion.contains(EpsilonStr)) {
    // Contains epsilon, check if there are remaining symbols to add in the
    // production
    if (prodStatus.CurrentIndex < prodStatus.Prod.size()) {
      // Remove epsilon, we are going to append the next symbol
      prodStatus.Expansion.erase(EpsilonStr);

      // Add new symbol to expand the firsts
      std::string_view newSymbol = prodStatus.Prod[prodStatus.CurrentIndex];
      prodStatus.Expansion.insert(newSymbol);

      // Increment so next time we'll check for the next symbol
      prodStatus.CurrentIndex++;

      // Symbol added
      return true;
    }
  }

  // Not symbol added
  return false;
}

static bool TryResolveProd(const Rules &rules,
                           StatusOfRules<ProductionStatus> &rulesStatus,
                           ProductionStatus &prodStatus) {

  if (AreAllTerminal(rules, prodStatus)) {
    // If added symbol, then it is not done
    return !AddNextSymbolInProdIfContainsEpsilon(prodStatus);
  }

  // Not all are terminal, so try to insert the symbols from the non-terminal
  // if it has the rule done
  for (std::string_view sym : prodStatus.Expansion) {

    // If the production is not done, then we can't insert its symbols, skip
    if (IsTerminal(rules, sym.data()) || !rulesStatus[sym].Done) {
      continue;
    }

    // The productions is done for the non terminal, remove it
    prodStatus.Expansion.erase(sym);

    // And add new symbols from the rule to expand the firsts
    for (ProductionStatus &prods : rulesStatus[sym].ProductionsStatus) {
      prodStatus.Expansion.insert(prods.Expansion.begin(),
                                  prods.Expansion.end());
    }

    break;
  }

  // Prod not done
  return false;
}

/// Returns true when it accomplished to resolve the whole Rule
static bool TryResolveRule(const Rules &rules,
                           std::vector<ProductionStatus> &statusByProd,
                           StatusOfRules<ProductionStatus> &rulesStatus) {
  bool allProdDone = true;
  for (ProductionStatus &prodStatus : statusByProd) {
    // Production done, skip
    if (prodStatus.IsDone) {
      continue;
    }

    prodStatus.IsDone = TryResolveProd(rules, rulesStatus, prodStatus);

    // Accumulate done productions
    allProdDone &= prodStatus.IsDone;
  }

  return allProdDone;
}

static StatusOfRules<ProductionStatus>
InitializeStatusTrackerOfRules(const Rules &rules) {
  StatusOfRules<ProductionStatus> initData;
  for (auto &[symbol, prods] : rules) {
    for (const Production &prod : prods) {
      initData[symbol].ProductionsStatus.emplace_back(prod, prod.front());
    }
  }

  return initData;
}

static FirstSet
BuildFirstSet(const StatusOfRules<ProductionStatus> &statusOfRules) {
  FirstSet result;

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

std::optional<FirstSet> ffps::BuildFirstSet(const Rules &rules) {
  // Initialize first tracking data
  StatusOfRules<ProductionStatus> rulesStatus =
      InitializeStatusTrackerOfRules(rules);

  while (!AreAllFirstDone(rulesStatus)) {
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
          TryResolveRule(rules, ruleStatus.ProductionsStatus, rulesStatus);
    }
  }

  return ::BuildFirstSet(rulesStatus);
}

void ffps::Print(const FirstSet &firstSet, std::ostream &out) {
  for (auto &[symbol, firsts] : firstSet) {
    out << symbol << "\n\t";
    for (auto &sym : firsts) {
      out << sym << " ";
    }
    out << "\n";
  }
}
