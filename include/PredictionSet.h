#ifndef __PREDICTIONSET_H__
#define __PREDICTIONSET_H__

#include <Common.h>
#include <First.h>
#include <Following.h>
#include <Rules.h>

namespace ffps {

using PredictionSet = Rules;

/// Produces the prediction set, from a precalculated set of rules. It
/// calcaulates the first and following set internally
std::optional<ffps::PredictionSet> BuildPredictionSet(const Rules &rules);

/// Produces the prediction set, from a precalculated set of rules, first, and
/// following
ffps::PredictionSet BuildPredictionSet(const Rules &rules,
                                       const FirstSet &firstSet,
                                       const FollowingSet &followingSet);

void Print(const PredictionSet &predictionSet, std::ostream &out);

} // namespace ffps

#endif // __PREDICTIONSET_H__
