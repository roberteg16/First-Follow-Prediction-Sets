#ifndef __PREDICTIONSET_H__
#define __PREDICTIONSET_H__

#include <Common.h>
#include <Rules.h>
#include <First.h>
#include <Following.h>

class PredictionSetGenerator
{
  public:
    PredictionSetGenerator(const Rules &rules, const Result &first,
                           const Result &following)
        : Rules(rules), First(first), Following(following) {
      // Init data
      init();
      process_production_set();
    }

    void print_rules();
    void print_first();
    void print_following();
    void print_prediction_set();

    void write_rules_to_file(std::string_view file, const Rules &);

    const Rules &get_rules() { return Rules; }
    const Result &get_first() { return First; }
    const Result &get_following() { return Following; }
    ProductionSet& get_production_set() { return production_set; }

    template<class T>
    void writeValue(T value, std::ostream& f) const{
    	f.write(reinterpret_cast<char*> (&value), sizeof(value));
    }
    void writeString(const std::string& s, std::ostream& f) const;

    template<typename T = std::ostream>
    void print_production_set(T& out = std::cout)
    {
      for(auto e : production_set)
      {
        out << e.first << '\n';
        for(auto& v : e.second)
        {
          out << '\t';
          for(auto& ve : v)
            out << ve << ' ';
          out << '\n';
        }
        out << '\n';
      }
    }

  private:
    void init();
    void process_production_set();
    std::set<std::string> first_of_production(const Production &);

    const Rules &Rules;
    const Result &First;
    const Result &Following;

    //Result of PredictionSet
    ProductionSet production_set;
};
#endif
