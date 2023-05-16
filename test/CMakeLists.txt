add_executable(main
   src/main.cpp

   src/errors/sqlite.cpp
   src/connection.cpp
   src/statement.cpp
)

target_link_libraries(main PRIVATE library)

target_compile_features(main PRIVATE cxx_std_17)

target_include_directories(main
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)
