add_executable(main main.cpp)

target_link_libraries(main PRIVATE library)

set_target_properties(main PROPERTIES
   CXX_STANDARD 11
   CXX_STANDARD_REQUIRED YES
   CXX_EXTENSIONS NO
)
