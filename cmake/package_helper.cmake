include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "sqlite-burrito")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)

# Shared/static suffix
if(BUILD_SHARED_LIBS)
    set(LIBRARY_TYPE "shared")
else()
    set(LIBRARY_TYPE "static")
endif()

# User-friendly CPU architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        set(CPU_ARCH "x64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(CPU_ARCH "arm64")
    else()
        set(CPU_ARCH "64bit")
    endif()
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86")
        set(CPU_ARCH "x86")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
        set(CPU_ARCH "arm32")
    else()
        set(CPU_ARCH "32bit")
    endif()
endif()

# User-friendly OS name
if(WIN32)
    set(OS_NAME "windows")
elseif(APPLE)
    set(OS_NAME "macos")
elseif(UNIX)
    set(OS_NAME "linux")
else()
    set(OS_NAME ${CMAKE_SYSTEM_NAME})
    string(TOLOWER ${OS_NAME} OS_NAME)
endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${OS_NAME}-${CPU_ARCH}-${LIBRARY_TYPE}")

include(CPack)