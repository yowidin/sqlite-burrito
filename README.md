![release](https://github.com/yowidin/sqlite-burrito/actions/workflows/release.yml/badge.svg)

# SQLiteBurrito

SQLiteBurrito is a lightweight C++ library that serves as a wrapper around SQLite3, providing an easy-to-use interface 
for working with SQLite databases. It aims to simplify the process of interacting with SQLite and offers a more 
intuitive and modern approach compared to the raw SQLite API.

## Features

- **Error Handling Options**: SQLiteBurrito provides flexibility in error handling by allowing users to choose between 
two error handling mechanisms: exceptions and `std::error_code`.

- **Optional Support**: SQLiteBurrito allows the use of `std::optional` to store and retrieve data from the database, 
providing an abstraction for `NULL` values.

## Getting Started

### Prerequisites

- C++17 compatible compiler
- SQLite3 library (either installed in your system, or as a conan dependency).

### Installation

The simplest way to include this library in your project is to use the CMake's FetchContent functionality, 
for example:

```CMake
include(FetchContent)
FetchContent_Declare(
   sqlite_burrito
   GIT_REPOSITORY https://github.com/yowidin/sqlite-burrito
   GIT_TAG release/x.y.z
)
FetchContent_MakeAvailable(sqlite_burrito)

target_link_libraries(my_app PUBLIC SQLiteBurrito::library)
```

### Usage

See the [example](example) directory contents for both exception-based and `std::error_code`-based usage examples.


### License

This project is licensed under the [MIT License](LICENSE).