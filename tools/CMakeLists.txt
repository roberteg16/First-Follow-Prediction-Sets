add_executable(PredictionSetExe main.cpp)

target_link_libraries(PredictionSetExe ProductionSet)

target_compile_features(PredictionSetExe PUBLIC cxx_std_20)
