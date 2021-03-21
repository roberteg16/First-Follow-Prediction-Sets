#include <First.h>
#include <Following.h>
#include <PredictionSet.h>
#include <Rules.h>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cout << "./predictionSet GrammarFile RulesOutFile PredictionSetOutFile"
              << std::endl;
    exit(-1);
  }

  const std::string_view grammarFile = argv[1];
  std::optional<ffps::Rules> rules = ffps::BuildRules(grammarFile, true, true);
  if (!rules) {
    return -1;
  }

  std::cout << "================ Rules ================\n";
  ffps::Print(*rules, std::cout);
  std::cout << "=======================================\n\n";

  std::optional<ffps::FirstSet> firstSet = ffps::BuildFirstSet(*rules);
  if (!firstSet) {
    return -1;
  }

  std::cout << "================ First ================\n";
  ffps::Print(*firstSet, std::cout);
  std::cout << "=======================================\n\n";

  std::optional<ffps::FollowingSet> followingSet =
      ffps::BuildFollowingSet(*rules, *firstSet);
  if (!followingSet) {
    return -1;
  }

  std::cout << "============== Following ==============\n";
  ffps::Print(*followingSet, std::cout);
  std::cout << "=======================================\n";

  return 0;
}
