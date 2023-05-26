cmake_minimum_required(VERSION 3.14)

set(SB_SOVERSION 0)
mark_as_advanced(SB_SOVERSION)

set(SB_VERSION 0.2.3)
mark_as_advanced(SB_VERSION)

project(SQLiteBurrito VERSION ${SB_VERSION} LANGUAGES CXX)

find_package(SQLite3 REQUIRED)
message(STATUS "SQLite3 version: ${SQLite3_VERSION}")

# --- Files generation -- #

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(SB_GENERATED_FILES_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(SB_GENERATED_INCLUDES_DIR "${SB_GENERATED_FILES_DIR}/include")
set(SB_GENERATED_SRC_DIR "${SB_GENERATED_FILES_DIR}/src")
set(SB_GENERATED_CMAKE_DIR "${SB_GENERATED_FILES_DIR}/cmake")
set(SB_GENERATED_CONFIG_HEADER "${SB_GENERATED_INCLUDES_DIR}/sqlite-burrito/config.h")
set(SB_GENERATED_EXPORT_HEADER "${SB_GENERATED_INCLUDES_DIR}/sqlite-burrito/export.h")

file(MAKE_DIRECTORY ${SB_GENERATED_INCLUDES_DIR})
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config.h.in ${SB_GENERATED_CONFIG_HEADER})

# --- Library and its properties --- #

add_library(library
   src/errors/sqlite.cpp
   src/connection.cpp
   src/statement.cpp
   src/transaction.cpp
   src/versioned_database.cpp
)

target_compile_features(library PUBLIC cxx_std_17)

set_target_properties(library PROPERTIES
   VISIBILITY_INLINES_HIDDEN ON
   CXX_VISIBILITY_PRESET hidden

   SOVERSION ${SB_SOVERSION}
   VERSION ${SB_VERSION}

   OUTPUT_NAME SQLiteBurrito
)

if(NOT APPLE)
   set_target_properties(library PROPERTIES INSTALL_RPATH $ORIGIN)
endif()

target_include_directories(library
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
   PUBLIC $<BUILD_INTERFACE:${SB_GENERATED_INCLUDES_DIR}>

   PUBLIC $<INSTALL_INTERFACE:include/>
)

target_link_libraries(library PUBLIC SQLite::SQLite3)

# Testing
if(BUILD_TESTING)
   include(CTest)
   add_subdirectory(test)
endif()

# Samples
option(BUILD_EXAMPLES "Build usage examples" OFF)
if(BUILD_EXAMPLES)
   add_subdirectory(example)
endif()

# Configure installation settings
include(GenerateExportHeader)
generate_export_header(
   library
   BASE_NAME SQLITE_BURRITO EXPORT_FILE_NAME
   ${SB_GENERATED_EXPORT_HEADER}
)

include(cmake/install_helper.cmake)
include(cmake/package_helper.cmake)