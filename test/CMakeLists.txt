add_executable(main
   src/main.cpp

   src/errors/sqlite.cpp
   src/connection.cpp
   src/statement.cpp
   src/bind_parameters.cpp
)

target_link_libraries(main PRIVATE library)

set_target_properties(main PROPERTIES
   CXX_STANDARD 17
   CXX_STANDARD_REQUIRED YES
   CXX_EXTENSIONS NO
)

target_include_directories(main
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)
