cmake_minimum_required(VERSION 3.14)

set(SQLITE_BURRITO_SOVERSION 0)
mark_as_advanced(SQLITE_BURRITO_SO_VERSION)

set(SQLITE_BURRITO_VERSION 0.1.0)
mark_as_advanced(SQLITE_BURRITO_VERSION)

project(SQLiteBurrito VERSION ${SQLITE_BURRITO_VERSION} LANGUAGES CXX)

find_package(SQLite3 REQUIRED)
message(STATUS "SQLite3 version: ${SQLite3_VERSION}")

# --- Files generation -- #

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(SB_GENERATED_FILES_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(SB_GENERATED_INCLUDES_DIR "${SB_GENERATED_FILES_DIR}/include")
set(SB_GENERATED_SRC_DIR "${SB_GENERATED_FILES_DIR}/src")
set(SB_GENERATED_CONFIG_HEADER  "${SB_GENERATED_INCLUDES_DIR}/sqlite-burrito/config.h")
set(SB_GENERATED_VERSION_HEADER "${SB_GENERATED_INCLUDES_DIR}/sqlite-burrito/version.h")
set(SB_GENERATED_EXPORT_HEADER  "${SB_GENERATED_INCLUDES_DIR}/sqlite-burrito/export.h")

file(MAKE_DIRECTORY ${SB_GENERATED_INCLUDES_DIR})
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config.h.in ${SB_GENERATED_CONFIG_HEADER})
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/version.h.in ${SB_GENERATED_VERSION_HEADER})

# --- Library and its properties --- #

add_library(library
   include/sqlite-burrito/library.h

   src/library.cpp
   src/errors/sqlite.cpp
)

# --- Generate (or use an existing) isolation header and source files --- #
set(SB_ISOLATION_HEADER_FILE ${SB_GENERATED_INCLUDES_DIR}/sqlite-burrito/isolation/sqlite3.h)
set(SB_ISOLATION_SOURCE_FILE ${SB_GENERATED_SRC_DIR}/isolation/sqlite3.cpp)

option(SB_USE_EXISTING_ISOLATION_FILES "Use already existing isolation files" OFF)
mark_as_advanced(SB_USE_EXISTING_ISOLATION_FILES)

if(NOT SB_USE_EXISTING_ISOLATION_FILES)
   message(STATUS "Generating isolation files")
   set(SQLITE3_INCLUDE_FILE_PATH ${SQLite3_INCLUDE_DIRS}/sqlite3.h)
   set(GIF_GENERATOR_PATH ${CMAKE_CURRENT_LIST_DIR}/tools/generate-isolation-files.py)
   include(${CMAKE_CURRENT_LIST_DIR}/cmake/GenerateIsolationFiles.cmake)
   generate_isolation_files(
      GENERATOR_PATH ${GIF_GENERATOR_PATH}
      INCLUDE_FILE ${SQLITE3_INCLUDE_FILE_PATH}
      INCLUDE_OUTPUT_PATH ${SB_ISOLATION_HEADER_FILE}
      SOURCE_OUTPUT_PATH ${SB_ISOLATION_SOURCE_FILE}
   )

   # Add the generated files as dependencies for the main application
   add_custom_target(SQLiteBurritoIsolationFiles
      DEPENDS ${SB_ISOLATION_HEADER_FILE} ${SB_ISOLATION_SOURCE_FILE} ${GIF_GENERATOR_PATH}
      SOURCES ${SQLITE3_INCLUDE_FILE_PATH})
   add_dependencies(library SQLiteBurritoIsolationFiles)
else()
   message(STATUS "Using existing isolation files")
endif()

message(STATUS "Isolation files:")
message(STATUS "${SB_ISOLATION_HEADER_FILE}")
message(STATUS "${SB_ISOLATION_SOURCE_FILE}")

target_sources(library PRIVATE ${SB_ISOLATION_SOURCE_FILE})

set_target_properties(library PROPERTIES
   VISIBILITY_INLINES_HIDDEN ON
   CXX_VISIBILITY_PRESET hidden

   SOVERSION ${SQLITE_BURRITO_SOVERSION}
   VERSION ${SQLITE_BURRITO_VERSION}

   CXX_STANDARD 11
   CXX_STANDARD_REQUIRED YES
   CXX_EXTENSIONS NO

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

# --- Installation and export --- #

include(GenerateExportHeader)
generate_export_header(library
   BASE_NAME SQLITE_BURRITO
   EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/generated/include/sqlite-burrito/export.h
)

set(SQLiteBurrito_GENERATED_CMAKE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/cmake")
set(SQLiteBurrito_VERSION_CONFIG "${SQLiteBurrito_GENERATED_CMAKE_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(SQLiteBurrito_PROJECT_CONFIG "${SQLiteBurrito_GENERATED_CMAKE_DIR}/${PROJECT_NAME}Config.cmake")

set(SQLiteBurrito_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "Path to the SQLiteBurrito cmake files")

write_basic_package_version_file(${SQLiteBurrito_VERSION_CONFIG} COMPATIBILITY SameMajorVersion)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/Config.cmake.in
   ${SQLiteBurrito_PROJECT_CONFIG}
   INSTALL_DESTINATION ${SQLiteBurrito_INSTALL_CMAKEDIR}
)

install(TARGETS library EXPORT SQLiteBurrito_Targets
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
           COMPONENT   SQLiteBurrito_Runtime
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
           COMPONENT          SQLiteBurrito_Runtime
           NAMELINK_COMPONENT SQLiteBurrito_Development
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
           COMPONENT   SQLiteBurrito_Development
   INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
   FILES
      ${SB_GENERATED_CONFIG_HEADER}
      ${SB_GENERATED_VERSION_HEADER}
      ${SB_GENERATED_EXPORT_HEADER}
   DESTINATION
      "${CMAKE_INSTALL_INCLUDEDIR}/sqlite-burrito"
)

install(FILES ${SQLiteBurrito_PROJECT_CONFIG} ${SQLiteBurrito_VERSION_CONFIG}
   DESTINATION ${SQLiteBurrito_INSTALL_CMAKEDIR}
)
install(EXPORT SQLiteBurrito_Targets
   DESTINATION ${SQLiteBurrito_INSTALL_CMAKEDIR}
   NAMESPACE SQLiteBurrito::
   FILE SQLiteBurrito-Targets.cmake
   COMPONENT SQLiteBurrito_Development
)

# --- Testing --- #

if(BUILD_TESTING)
   include(CTest)
   add_subdirectory(test)
endif()
