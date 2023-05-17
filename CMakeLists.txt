cmake_minimum_required(VERSION 3.14)

set(SQLITE_BURRITO_SOVERSION 0)
mark_as_advanced(SQLITE_BURRITO_SO_VERSION)

set(SQLITE_BURRITO_VERSION 0.2.0)
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
set(SB_GENERATED_EXPORT_HEADER  "${SB_GENERATED_INCLUDES_DIR}/sqlite-burrito/export.h")

file(MAKE_DIRECTORY ${SB_GENERATED_INCLUDES_DIR})
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config.h.in ${SB_GENERATED_CONFIG_HEADER})

# --- Library and its properties --- #

add_library(library
   src/errors/sqlite.cpp
   src/connection.cpp
   src/statement.cpp
)

target_compile_features(library PUBLIC cxx_std_17)

set_target_properties(library PROPERTIES
   VISIBILITY_INLINES_HIDDEN ON
   CXX_VISIBILITY_PRESET hidden

   SOVERSION ${SQLITE_BURRITO_SOVERSION}
   VERSION ${SQLITE_BURRITO_VERSION}

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
   RUNTIME DESTINATION        ${CMAKE_INSTALL_BINDIR}
           COMPONENT          SQLiteBurrito_Runtime
   LIBRARY DESTINATION        ${CMAKE_INSTALL_LIBDIR}
           COMPONENT          SQLiteBurrito_Runtime
           NAMELINK_COMPONENT SQLiteBurrito_Development
   ARCHIVE DESTINATION        ${CMAKE_INSTALL_LIBDIR}
           COMPONENT          SQLiteBurrito_Development
   INCLUDES DESTINATION       ${CMAKE_INSTALL_INCLUDEDIR}
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
