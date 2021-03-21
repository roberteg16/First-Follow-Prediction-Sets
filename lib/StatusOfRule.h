#ifndef __LIB_STATUSOFRULE_H__
#define __LIB_STATUSOFRULE_H__

#include <string_view>
#include <unordered_map>
#include <vector>

template <typename ProductionStatus_t> struct RuleStatus {
  /// Tracks whether the rule is done
  bool Done;
  /// Tracks the current status of all the productions of this rule
  std::vector<ProductionStatus_t> ProductionsStatus;
};
template <typename ProductionStatus_t>
using StatusOfRules =
    std::unordered_map<std::string_view, RuleStatus<ProductionStatus_t>>;

#endif // __LIB_STATUSOFRULE_H__