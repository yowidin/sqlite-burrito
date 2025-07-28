include(FetchContent)

FetchContent_Declare(
   Catch2
   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
   GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(main
   src/errors/sqlite.cpp
   src/connection.cpp
   src/empty_arrays.cpp
   src/statement.cpp
   src/transaction.cpp
   src/versioned_database.cpp
)

target_link_libraries(main PRIVATE library Catch2::Catch2WithMain)

target_compile_features(main PRIVATE cxx_std_17)

add_test(NAME catch2 COMMAND main WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
