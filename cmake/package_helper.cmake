include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "sqlite-burrito")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)

include(CPack)