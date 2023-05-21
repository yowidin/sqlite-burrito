add_executable(exceptions-example main.cpp)
target_link_libraries(exceptions-example PRIVATE library)
target_compile_features(exceptions-example PRIVATE cxx_std_17)
