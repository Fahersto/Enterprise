# Enterprise Launcher

# Source files
set(LAUNCHER_SRC
)
set(LAUNCHER_SRC_WIN32
    src/EntryPoint_Win32.cpp
)
set(LAUNCHER_SRC_MACOS
    src/EntryPoint_macOS.mm
)

# Resources (ATTN: all entries below must use absolute paths!)
set(LAUNCHER_RESOURCES_WIN32
    "${CMAKE_SOURCE_DIR}/Editor/resources/EmbedAppIcon.rc"
    "${CMAKE_SOURCE_DIR}/Editor/resources/Enterprise.ico"
)

# Targets
add_executable(Launcher ${LAUNCHER_SRC} ${LAUNCHER_SRC_WIN32} ${LAUNCHER_SRC_MACOS} ${LAUNCHER_RESOURCES_WIN32})
set_target_properties(Launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

# Don't compile src for other platforms
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set_source_files_properties(${LAUNCHER_SRC_MACOS}
		PROPERTIES HEADER_FILE_ONLY TRUE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set_source_files_properties(${LAUNCHER_SRC_WIN32}
		PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

# Dependency linking
target_link_libraries(Launcher Engine_DYN)

# Build flags/settings
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(Launcher PROPERTIES
        WIN32_EXECUTABLE TRUE
)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Set up relative path to libEngine.dylib
    add_custom_command (TARGET Launcher
        POST_BUILD COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
            "-change" "@rpath/libEngine.dylib" "@executable_path/../Frameworks/libEngine.dylib"
            "$<TARGET_FILE:Launcher>" VERBATIM
    )
endif()

# IDE project organization
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "src"
    FILES ${LAUNCHER_SRC} ${LAUNCHER_SRC_WIN32} ${LAUNCHER_SRC_MACOS}
)
source_group(TREE "${CMAKE_SOURCE_DIR}/Editor/resources"
    PREFIX "resources"
    FILES ${LAUNCHER_RESOURCES_WIN32}
)

# Debugger setup
set_target_properties(Launcher PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    VS_DEBUGGER_COMMAND_ARGUMENTS "-c \"Editor/content\" -d \"data/Editor\" -e \"Engine/include_glsl\""
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    XCODE_SCHEME_ARGUMENTS "-c \"Editor/content\";-d \"data/Editor\";-e \"Engine/include_glsl\""
)
