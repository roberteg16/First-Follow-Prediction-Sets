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

  FirstGenerator firstGenerator(*rules);
  FollowingGenerator followingGenerator(*rules, firstGenerator.get_first());
  PredictionSetGenerator predictionSetGenerator(
      *rules, firstGenerator.get_first(), followingGenerator.get_following());

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

  return 0;
}
