#ifndef __LIB_CICLEDETECTOR_H__
#define __LIB_CICLEDETECTOR_H__

#include "StatusOfRule.h"

#include <iostream>

/// Halt criterium. If we are trying to obtain the first set of
/// ourself. This generates a infinite cycle.
template <typename ProductionStatus_t>
static bool
FirstSetOfItselfCycle(const StatusOfRules<ProductionStatus_t> &statusOfRules) {
  for (auto &[symbol, ruleStatus] : statusOfRules) {
    if (ruleStatus.Done) {
      continue;
    }
    for (auto &productionStatus : ruleStatus.ProductionsStatus) {
      if (productionStatus.Done) {
        continue;
      }
      if (productionStatus.Expansion.contains(symbol)) {
        std::cerr << "Your gramma makes the first set of symbol '" << symbol
                  << "' to contain itself. Not able to generate set due"
                  << " to infinite cycle.\n";
        return true;
      }
    }
  }
  return false;
}

/// Halt criterium. If we are trying to obtain the first set of
///.another symbol, and it happens that that symbol is also trying to obtain
/// our first set. This creates a infiniti cycle.
template <typename ProductionStatus_t>
static bool FirstSetOfAnotherFirstSetOfUsCycle(
    const StatusOfRules<ProductionStatus_t> &statusOfRules) {

  for (auto &[symbol, ruleStatus] : statusOfRules) {
    if (ruleStatus.Done) {
      continue;
    }
    for (auto &[symbol2, ruleStatus2] : statusOfRules) {
      if (ruleStatus.Done || symbol == symbol2) {
        continue;
      }

      for (auto &productionStatus : ruleStatus2.ProductionsStatus) {
        if (productionStatus.Done) {
          continue;
        }

        if (productionStatus.Expansion.contains(symbol)) {
          for (auto &productionStatus : ruleStatus.ProductionsStatus) {
            if (productionStatus.Done) {
              continue;
            }

            if (productionStatus.Expansion.contains(symbol2)) {
              std::cerr
                  << "Your gramma contains a infinite cycle on rules: '"
                  << symbol << "' and '" << symbol2
                  << "'. Not able to generate set due to infinite cycle\n";
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

#endif // __LIB_CICLEDETECTOR_H__