# Enterprise TestGame

# Source files
set(GAME_SRC
    "src/TestGame.cpp"
)

# Resources (ATTN: all entries below must use absolute paths!)
set(GAME_RESOURCES_WIN32
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/EmbedAppIcon.rc"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/TestGame.ico"
)
set(GAME_RESOURCES_MACOS
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/TestGame.icns"
)
set(GAME_CONTENT
    "${CMAKE_CURRENT_SOURCE_DIR}/content/Example.txt"
)

# Targets
add_library(TestGame_DYN MODULE ${GAME_SRC})
add_executable(TestGame ${GAME_SRC} ${GAME_RESOURCES_WIN32} ${GAME_RESOURCES_MACOS} ${GAME_CONTENT})
set_target_properties(TestGame_DYN PROPERTIES
    OUTPUT_NAME TestGame
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/TestGame/module"
)
set_target_properties(TestGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/TestGame/standalone"
)

# Dependency linking
target_link_libraries(TestGame_DYN Engine_DYN)
target_link_libraries(TestGame Engine_LIB)

# Build flags/settings
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(TestGame PROPERTIES
        WIN32_EXECUTABLE TRUE
)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Set up bundle
    set_target_properties(TestGame PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "TestGame"
        MACOSX_BUNDLE_COPYRIGHT "${EP_COPYRIGHT_NOTICE}"
        MACOSX_BUNDLE_ICON_FILE "TestGame"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${EP_APPLE_ORGID}.TestGame"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${EP_APPLE_ORGID}.TestGame"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    )

    # Copy resources to app bundle
    set_source_files_properties(${GAME_RESOURCES_MACOS} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    foreach(ENGINE_GLSL_HEADER ${ENGINE_GLSL_HEADERS})
        add_custom_command(TARGET TestGame POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/Engine/${ENGINE_GLSL_HEADER}"
            "$<TARGET_BUNDLE_CONTENT_DIR:TestGame>/Resources/${ENGINE_GLSL_HEADER}"
        )
    endforeach()

    # Copy content folder to app bundle
    foreach(RESOURCE_FILE ${GAME_CONTENT})
        cmake_path(RELATIVE_PATH RESOURCE_FILE BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE RESOURCE_FILE_PATH)
        cmake_path(GET RESOURCE_FILE_PATH PARENT_PATH RESOURCE_FILE_FOLDER_PATH)
        set_property(SOURCE ${RESOURCE_FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${RESOURCE_FILE_FOLDER_PATH}")
    endforeach(RESOURCE_FILE)
endif()

# Enable link-time optimization
if(LTO_SUPPORTED)
    set_target_properties(TestGame PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_DEV TRUE
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    )
endif()

# IDE project organization
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "src"
    FILES ${GAME_SRC}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/content"
    PREFIX "content"
    FILES ${GAME_CONTENT}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    PREFIX "resources"
    FILES ${GAME_RESOURCES_WIN32} ${GAME_RESOURCES_MACOS}
)

# Debugger setup
set_target_properties(TestGame PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    VS_DEBUGGER_COMMAND_ARGUMENTS "-c \"Editor/content\" -d \"data/Editor\" -e \"Engine/include_glsl\""
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    XCODE_SCHEME_ARGUMENTS "-c \"Editor/content\";-d \"data/Editor\";-e \"Engine/include_glsl\""
)

# Install locations
option(EP_INSTALL_TESTGAME "If set to ON, the standalone TestGame will be installed when using --install." OFF)
if(EP_INSTALL_TESTGAME)
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        install(TARGETS TestGame DESTINATION "TestGame")
        install(DIRECTORY "${CMAKE_SOURCE_DIR}/Engine/include_glsl" "${CMAKE_CURRENT_SOURCE_DIR}/content" DESTINATION "TestGame")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        install(TARGETS TestGame DESTINATION ".")
        install (CODE "
            include(BundleUtilities)
            fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/TestGame.app\" \"\" \"\")
            "
        )
    endif()
endif()
