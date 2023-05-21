add_executable(errors-example main.cpp)
target_link_libraries(errors-example PRIVATE library)
target_compile_features(errors-example PRIVATE cxx_std_17)
