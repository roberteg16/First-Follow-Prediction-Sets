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
  std::optional<Rules> rules = ffps::BuildRules(grammarFile, true, true);
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
  std::cout << "=======================================\n";


/*
  FollowingGenerator followingGenerator(*rules, firstSet);
  PredictionSetGenerator predictionSetGenerator(
      *rules, firstSet, followingGenerator.get_following());


  predictionSetGenerator.write_rules_to_file(
      argv[2], predictionSetGenerator.get_rules());
  predictionSetGenerator.write_rules_to_file(
      argv[3], predictionSetGenerator.get_production_set());

  predictionSetGenerator.print_rules();
  std::cout << std::endl;
  std::cout << std::endl;
  predictionSetGenerator.print_first();
  std::cout << std::endl;
  std::cout << std::endl;
  predictionSetGenerator.print_following();
  std::cout << std::endl;
  std::cout << std::endl;
  predictionSetGenerator.print_prediction_set();
*/
  return 0;
}
