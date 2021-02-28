add_library(ProductionSet STATIC
    Common.cpp
    First.cpp
    Following.cpp
    PredictionSet.cpp
    Rules.cpp
)

target_compile_features(ProductionSet PRIVATE cxx_std_20)
