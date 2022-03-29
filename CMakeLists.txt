# Enterprise Development Project (top level)

cmake_minimum_required(VERSION 3.23)
include(CheckIPOSupported)

# Global vars
set(EP_COPYRIGHT_NOTICE "Copyright © 2022 Michael Martz. All rights reserved.")
set(EP_VERSION "0.1.0.0")
set(EP_MACOS_MINVERSION "10.15")
set(EP_APPLE_ORGID com.MichaelMartz)

set(CMAKE_CONFIGURATION_TYPES "Debug;Dev;Release"
    CACHE STRING "Semicolon-separated list of supported configuration types."
)
set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "${EP_MACOS_MINVERSION}")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
set(CMAKE_SKIP_RPATH TRUE CACHE BOOL "If set, runtime paths are not added when using shared libraries.")

# Project declaration
project(Enterprise VERSION ${EP_VERSION} LANGUAGES C CXX)
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    enable_language(OBJC)
    enable_language(OBJCXX)
endif()

# Platform check
if(NOT CMAKE_SYSTEM_NAME MATCHES "Windows" AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(FATAL_ERROR "Enterprise only supports Windows and Mac!")
elseif(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Enterprise requires a 64-bit architecture!")
endif()

# Link-time optimization check
check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_UNSUPPORTED_ERROR)
if(NOT LTO_SUPPORTED)
    message(WARNING "Link-time optimization is not supported by ${CMAKE_CXX_COMPILER}!  Error: ${LTO_UNSUPPORTED_ERROR}")
endif()

# Dev configuration flags
if(MSVC)
    message(STATUS "Setting Dev configuration compiler flags for MSVC...")
    set(CMAKE_C_FLAGS_DEV "/Zi /O2 /Ob1 /DNDEBUG" CACHE STRING "Flags used by the C compiler during DEV builds." FORCE)
    set(CMAKE_CXX_FLAGS_DEV "/Zi /O2 /Ob1 /DNDEBUG" CACHE STRING "Flags used by the CXX compiler during DEV builds." FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_DEV "/debug /INCREMENTAL" CACHE STRING "Flags used by the linker during DEV builds." FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_DEV "/debug /INCREMENTAL" CACHE STRING "Flags used by the linker during the creation of shared libraries during DEV builds." FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS_DEV "/debug /INCREMENTAL" CACHE STRING "Flags used by the linker during the creation of modules during DEV builds." FORCE)
    message(STATUS "Setting Dev configuration compiler flags for MSVC... ...done!")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Setting Dev configuration compiler flags for Clang...")
    set(CMAKE_C_FLAGS_DEV "-O2 -g -DNDEBUG" CACHE STRING "Flags used by the C compiler during DEV builds." FORCE)
    set(CMAKE_CXX_FLAGS_DEV "-O2 -g -DNDEBUG" CACHE STRING "Flags used by the CXX compiler during DEV builds." FORCE)
    set(CMAKE_OBJC_FLAGS_DEV "-O2 -g -DNDEBUG" CACHE STRING "Flags used by the OBJC compiler during DEV builds." FORCE)
    set(CMAKE_OBJCXX_FLAGS_DEV "-O2 -g -DNDEBUG" CACHE STRING "Flags used by the OBJCXX compiler during DEV builds." FORCE)
    message(STATUS "Setting Dev configuration compiler flags for Clang... ...done!")
else()
    message(WARNING "Non-MSVC, Non-Clang compiler detected!  Compiler and linker flags for Dev configuration must be set manually!")
endif()

# Set default installation location
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(CMAKE_INSTALL_PREFIX "$ENV{PROGRAMFILES}" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(CMAKE_INSTALL_PREFIX "/Applications" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    endif()
endif()

# Targets
add_subdirectory(Engine)
add_subdirectory(Launcher)
add_subdirectory(Editor)
add_subdirectory(TestGame)

# IDE organization
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)
