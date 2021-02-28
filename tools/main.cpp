#include "First.h"
#include "Following.h"
#include "PredictionSet.h"
#include "Rules.h"

int main(int argc, char *argv[]) {
  if (argc < 4) {
    cout << "./predictionSet GrammarFile RulesOutFile PredictionSetOutFile"
         << endl;
    exit(-1);
  }

  PredictionSetGenerator predictionSetGenerator(argv[1]);
  predictionSetGenerator.write_rules_to_file(
      argv[2], predictionSetGenerator.get_rules());
  predictionSetGenerator.write_rules_to_file(
      argv[3], predictionSetGenerator.get_production_set());

  return 0;
}
