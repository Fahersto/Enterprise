# Enterprise Engine Library

# Headers
set(ENGINE_HEADERS
    "include/Enterprise/Core.h"
    "include/Enterprise/Enterprise.h"

    # Core
    "include/Enterprise/Application/Application.h"
    "include/Enterprise/Core/Assertions.h"
    "include/Enterprise/Core/Console.h"
    "include/Enterprise/Core/ErrorMessageBox.h"
    "include/Enterprise/Core/Exceptions.h"
    "include/Enterprise/Core/HashNames.h"
    "include/Enterprise/Core/SharedLibraryExports.h"

    # Foundation Systems
    "include/Enterprise/Time/Time.h"
    "include/Enterprise/Events/Events.h"
    "include/Enterprise/File/File.h"
    "include/Enterprise/File/INIValueType.h"

    # Interface Systems
    "include/Enterprise/Input/Input.h"
    "include/Enterprise/Input/ControlIDs.h"
    "include/Enterprise/Graphics/Graphics.h"
    "include/Enterprise/Graphics/OpenGLHelpers.h"
    "include/Enterprise/Graphics/Window.h"

    # Behavior Systems
    "include/Enterprise/SceneManager/SceneManager.h"
    "include/Enterprise/StateManager/StateManager.h"

    # ECS Systems
    "include/Enterprise/Systems/Renderer2D.h"

    PARENT_SCOPE
)
set(ENGINE_HEADERS_WIN32
    "include/Enterprise/Core/Win32APIHelpers.h"
    PARENT_SCOPE
)
set(ENGINE_HEADERS_MACOS
    PARENT_SCOPE
)
set(ENGINE_PRECOMPILE_HEADERS
    # ATTN: Only C++ headers are allowed in this list!
    "<iostream>"
    "<fstream>"
    "<filesystem>"
    "<memory>"
    "<array>"
    "<tuple>"
    "<list>"
    "<forward_list>"
    "<algorithm>"
    "<set>"
    "<stack>"
    "<variant>"
    "<string>"
    "<wchar.h>"
    "<sstream>"
    "<functional>"
    "<map>"
    "<stdio.h>"
    "<fcntl.h>"
    "<exception>"
    "<csignal>"
    "<math.h>"
    
    "<prettyprint.hpp>"
)

# Source files
set(ENGINE_SRC
    # Core
    "src/Core/HashNames.cpp"
    "src/Application/Application.cpp"

    # Foundation Systems
    "src/Time/Time.cpp"
    "src/Events/Events.cpp"
    "src/File/File.cpp"
    "src/File/INIReader.cpp"
    "src/File/INIWriter.cpp"

    # Interface Systems
    "src/Input/Input.cpp"
    "src/Graphics/Graphics.cpp"
    "src/Graphics/Framebuffers.cpp"
    "src/Graphics/GlobalShaderVars.cpp"
    "src/Graphics/OpenGLHelpers.cpp"
    "src/Graphics/Shaders.cpp"
    "src/Graphics/Textures.cpp"
    "src/Graphics/VertexArrays.cpp"

    # Behavior Systems
    "src/SceneManager/SceneManager.cpp"
    "src/StateManager/StateManager.cpp"

    # ECS Systems
    "src/Systems/Renderer2D/Renderer2D.cpp"
    "src/Systems/Renderer2D/R2D_Sprites.cpp"
    "src/Systems/Renderer2D/R2D_Text.cpp"
)
set(ENGINE_SRC_WIN32
    "src/Core/Console_Win32.cpp"
    "src/Core/ErrorMessageBox_Win32.cpp"
    "src/Core/Win32APIHelpers.cpp"
    "src/Application/EntryPoint_Win32.cpp"

	"src/Time/Time_Win32.cpp"
	"src/File/File_Win32.cpp"

	"src/Input/Input_Win32.cpp"
	"src/Graphics/Window_Win32.cpp"
)
set(ENGINE_SRC_MACOS
    "src/Core/Console_macOS.cpp"
    "src/Core/ErrorMessageBox_macOS.mm"
    "src/Application/EntryPoint_macOS.mm"

	"src/Time/Time_macOS.cpp"
	"src/File/File_macOS.mm"

	"src/Input/Input_macOS.mm"
	"src/Graphics/Window_macOS.mm"
)

# Resources
set(ENGINE_GLSL_HEADERS
    "include_glsl/Enterprise.glsl"
    "include_glsl/BatchedTextures.glsl"
    "include_glsl/DistanceFields.glsl"

    PARENT_SCOPE
)

# Vendor
set(VENDOR_HEADERS
)
set(VENDOR_SRC
    "vendor/CTSpookyHash/src/CTSpooky.cpp"
    "vendor/Glad/src/glad.c"
    "vendor/stb_image/src/stb_image.cpp"
)

# Targets
add_library(Engine_DYN SHARED
    ${ENGINE_HEADERS} ${ENGINE_HEADERS_WIN32}
    ${ENGINE_SRC} ${ENGINE_SRC_WIN32} ${ENGINE_SRC_MACOS}
    ${ENGINE_GLSL_HEADERS}
	${VENDOR_HEADERS} ${VENDOR_SRC}
)
add_library(Engine_LIB STATIC
    ${ENGINE_HEADERS} ${ENGINE_HEADERS_WIN32}
    ${ENGINE_SRC} ${ENGINE_SRC_WIN32} ${ENGINE_SRC_MACOS}
    ${ENGINE_GLSL_HEADERS}
	${VENDOR_HEADERS} ${VENDOR_SRC}
)
set_target_properties(Engine_DYN PROPERTIES
    OUTPUT_NAME "Engine"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)
set_target_properties(Engine_LIB PROPERTIES
    OUTPUT_NAME "Engine"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/lib"
)

# Include dirs
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/CMakeLists.txt")
    message(FATAL_ERROR "The spdlog submodule is not downloaded!  Update submodules, then reattempt configuration.")
elseif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/CMakeLists.txt")
    message(FATAL_ERROR "The glm submodule is not downloaded!  Update submodules, then reattempt configuration.")
endif()
target_include_directories(Engine_DYN
    PUBLIC
        "include"
        "vendor/CTSpookyHash/include"
        "vendor/cxx-prettyprint"
        "vendor/Glad/include"
        "vendor/spdlog/include"
        "vendor/stb_image/include"
        "vendor/glm"
    PRIVATE
        "src"
)
target_include_directories(Engine_LIB
    PUBLIC
        "include"
        "vendor/CTSpookyHash/include"
        "vendor/cxx-prettyprint"
        "vendor/Glad/include"
        "vendor/spdlog/include"
        "vendor/stb_image/include"
        "vendor/glm"
    PRIVATE
        "src"
)

# Don't compile src for other platforms
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set_source_files_properties(${ENGINE_SRC_MACOS}
		PROPERTIES HEADER_FILE_ONLY TRUE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set_source_files_properties(${ENGINE_HEADERS_WIN32} ${ENGINE_SRC_WIN32}
		PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

# Don't compile developer resources
set_source_files_properties(${ENGINE_GLSL_HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

# CMake subprojects
# add_subdirectory(vendor/HotConsts)

# freetype (used by msdf-atlas-gen)
set(FT_DISABLE_ZLIB ON CACHE BOOL "")
set(FT_DISABLE_BZIP2 ON CACHE BOOL "")
set(FT_DISABLE_PNG ON CACHE BOOL "")
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "")
set(FT_DISABLE_BROTLI ON CACHE BOOL "")
add_subdirectory(vendor/freetype)
add_library(Freetype::Freetype ALIAS freetype)

# msdf-atlas-gen
set(MSDF_ATLAS_GEN_BUILD_STANDALONE OFF CACHE BOOL "")
add_subdirectory(vendor/msdf-atlas-gen)
set_source_files_properties(
    "vendor/msdf-atlas-gen/msdfgen/lib/lodepng.cpp"
    TARGET_DIRECTORY msdfgen-ext
    PROPERTIES COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
)
target_compile_definitions(msdf-atlas-gen PRIVATE _CRT_SECURE_NO_WARNINGS)

# yaml-cpp
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/yaml-cpp/CMakeLists.txt")
    message(FATAL_ERROR "The yaml-cpp submodule is not downloaded!  Please update submodules and try again.")
endif()
set(BUILD_TESTING OFF CACHE BOOL "Build the testing tree.")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Enable parse tools")
set(YAML_CPP_CLANG_FORMAT_TARGET OFF CACHE BOOL "Enable yaml-cpp's clang-format target")
add_subdirectory(vendor/yaml-cpp)

# Dependency linking
target_link_libraries(Engine_DYN
    PUBLIC
        yaml-cpp::yaml-cpp

        "$<$<PLATFORM_ID:Darwin>:-framework AppKit>"
    PRIVATE
        msdf-atlas-gen::msdf-atlas-gen

        "$<$<PLATFORM_ID:Windows>:xinput>"
        "$<$<PLATFORM_ID:Windows>:OpenGL32>"

        "$<$<PLATFORM_ID:Darwin>:-framework GameController>"
        "$<$<PLATFORM_ID:Darwin>:-framework Cocoa>"
        "$<$<PLATFORM_ID:Darwin>:-framework OpenGL>"
)
target_link_libraries(Engine_LIB
    PUBLIC
        yaml-cpp::yaml-cpp
        
    PRIVATE
        msdf-atlas-gen::msdf-atlas-gen

        "$<$<PLATFORM_ID:Windows>:xinput>"
        "$<$<PLATFORM_ID:Windows>:OpenGL32>"
        
        "$<$<PLATFORM_ID:Darwin>:-framework AppKit>"
        "$<$<PLATFORM_ID:Darwin>:-framework GameController>"
        "$<$<PLATFORM_ID:Darwin>:-framework Cocoa>"
        "$<$<PLATFORM_ID:Darwin>:-framework OpenGL>"
)

# Preprocessor defines
target_compile_definitions(Engine_DYN
    PUBLIC
        EP_BUILD_DYNAMIC
        $<$<CONFIG:Debug>:EP_CONFIG_DEBUG>
        $<$<CONFIG:Dev>:EP_CONFIG_DEV>
        $<$<CONFIG:Release>:EP_CONFIG_RELEASE>

        _USE_MATH_DEFINES
        $<$<PLATFORM_ID:Windows>:UNICODE>
        $<$<PLATFORM_ID:Windows>:_UNICODE>
        $<$<PLATFORM_ID:Windows>:NOMINMAX>
        $<$<PLATFORM_ID:Darwin>:GL_SILENCE_DEPRECATION>

        GLM_FORCE_LEFT_HANDED
        $<$<PLATFORM_ID:Windows>:YAML_CPP_STATIC_DEFINE>
    PRIVATE
        EP_BUILD_CORE
)
target_compile_definitions(Engine_LIB
    PUBLIC
        $<$<CONFIG:Debug>:EP_CONFIG_DEBUG>
        $<$<CONFIG:Dev>:EP_CONFIG_DEV>
        $<$<CONFIG:Release>:EP_CONFIG_RELEASE>
        
        _USE_MATH_DEFINES
        $<$<PLATFORM_ID:Windows>:UNICODE>
        $<$<PLATFORM_ID:Windows>:_UNICODE>
        $<$<PLATFORM_ID:Windows>:NOMINMAX>
        $<$<PLATFORM_ID:Darwin>:GL_SILENCE_DEPRECATION>

        GLM_FORCE_LEFT_HANDED
        $<$<PLATFORM_ID:Windows>:YAML_CPP_STATIC_DEFINE>
    PRIVATE
        EP_BUILD_CORE
)

# Build flags/settings
target_compile_features(Engine_DYN PUBLIC cxx_std_17)
target_compile_features(Engine_LIB PUBLIC cxx_std_17)
target_precompile_headers(Engine_DYN PRIVATE ${ENGINE_PRECOMPILE_HEADERS})
target_precompile_headers(Engine_LIB PRIVATE ${ENGINE_PRECOMPILE_HEADERS})
set_source_files_properties(${VENDOR_SRC} PROPERTIES SKIP_PRECOMPILE_HEADERS TRUE)

# Enable link-time optimization
if(LTO_SUPPORTED)
    set_target_properties(Engine_LIB PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_DEV TRUE
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    )
endif()

# IDE project organization
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/Enterprise"
	PREFIX "include"
	FILES ${ENGINE_HEADERS} ${ENGINE_HEADERS_WIN32}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
	PREFIX "src"
	FILES ${ENGINE_SRC} ${ENGINE_SRC_WIN32} ${ENGINE_SRC_MACOS}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/vendor"
	PREFIX "vendor"
	FILES ${VENDOR_HEADERS} ${VENDOR_SRC}
)
