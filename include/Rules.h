#ifndef __RULES_H__
#define __RULES_H__

#include <Common.h>
#include <string_view>
#include <optional>

namespace ffps {

/// In addition of extracting the rules from a file, if expecified it removes
/// LeftRecursion and/or CommonFactorsByTheLeft.
/// If the file does not exists or it fails at building the rules, it returns
/// a empty optional
std::optional<Rules> BuildRules(std::string_view file,
                                bool solveLeftRecursion = true,
                                bool solveCommonFactorsByTheLeft = true);

void Print(const Rules &rules, std::ostream &out);

}; // namespace ffps

#endif // __RULES_H__
