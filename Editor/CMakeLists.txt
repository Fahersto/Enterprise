# Enterprise Editor

# Source files
set(EDITOR_SRC
)
set(EDITOR_SRC_WIN32
    "src/EntryPoint_Win32.cpp"
)
set(EDITOR_SRC_MACOS
    "src/EntryPoint_macOS.mm"
)

# Resources (ATTN: all entries below must use absolute paths!)
set(EDITOR_RESOURCES_WIN32
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/EmbedAppIcon.rc"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/Enterprise.ico"
)
set(EDITOR_RESOURCES_MACOS
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/Enterprise.icns"
)
set(EDITOR_CONTENT
    "${CMAKE_CURRENT_SOURCE_DIR}/content/Example.txt"
)

# Targets
add_executable(Editor
	${EDITOR_SRC} ${EDITOR_SRC_WIN32} ${EDITOR_SRC_MACOS}
	${EDITOR_RESOURCES_WIN32} ${EDITOR_RESOURCES_MACOS} ${EDITOR_CONTENT})
set_target_properties(Editor PROPERTIES
    OUTPUT_NAME Enterprise
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)

# Don't compile src for other platforms
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_source_files_properties(${EDITOR_SRC_MACOS}
        PROPERTIES HEADER_FILE_ONLY TRUE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set_source_files_properties(${EDITOR_SRC_WIN32}
        PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

# Dependency linking
target_link_libraries(Editor Engine_DYN)

# Build flags/settings
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(Editor PROPERTIES
        WIN32_EXECUTABLE TRUE
)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Set up bundle
    set_target_properties(Editor PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "Enterprise"
        MACOSX_BUNDLE_COPYRIGHT "${EP_COPYRIGHT_NOTICE}"
        MACOSX_BUNDLE_ICON_FILE "Enterprise"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${EP_APPLE_ORGID}.Enterprise"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${EP_APPLE_ORGID}.Enterprise"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"

        XCODE_EMBED_FRAMEWORKS Engine_DYN
        XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY TRUE
        XCODE_ATTRIBUTE_COPY_PHASE_STRIP "NO"
    )

    # Copy resources to app bundle
    set_source_files_properties(${EDITOR_RESOURCES_MACOS} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    # Copy content folder to app bundle
    foreach(RESOURCE_FILE ${EDITOR_CONTENT})
        cmake_path(RELATIVE_PATH RESOURCE_FILE BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE RESOURCE_FILE_PATH)
        cmake_path(GET RESOURCE_FILE_PATH PARENT_PATH RESOURCE_FILE_FOLDER_PATH)
        set_property(SOURCE ${RESOURCE_FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${RESOURCE_FILE_FOLDER_PATH}")
    endforeach(RESOURCE_FILE)

    # Copy developer resources
    foreach(ENGINE_HEADER_FILE ${ENGINE_HEADERS})
        add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/Engine/${ENGINE_HEADER_FILE}"
            "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/${ENGINE_HEADER_FILE}"
        )
    endforeach()
    foreach(ENGINE_GLSL_HEADER ${ENGINE_GLSL_HEADERS})
        add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/Engine/${ENGINE_GLSL_HEADER}"
            "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/${ENGINE_GLSL_HEADER}"
        )
    endforeach()
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/LICENSE.txt"
        "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/licenses/Enterprise.txt"
    )

    # glm
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Engine/vendor/glm/glm"
        "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/include/glm"
    )
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/Engine/vendor/glm/copying.txt"
        "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/licenses/glm.txt"
    )

    # yaml-cpp
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Engine/vendor/yaml-cpp/include"
        "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/include"
    )
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/Engine/vendor/yaml-cpp/LICENSE"
        "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/licenses/yaml-cpp.txt"
    )

    # Embed other targets into bundle
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:Engine_LIB>" "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/libEngine.a"
    )
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:yaml-cpp>" "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Resources/developer/libyaml-cppd.a"
    )
    add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:Launcher>" "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/MacOS/Launcher"
    )
    add_custom_command(TARGET Editor POST_BUILD COMMAND "${CMAKE_INSTALL_NAME_TOOL}" "-change"
        "@executable_path/libEngine.dylib" "@executable_path/../Frameworks/libEngine.dylib"
        "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/MacOS/Launcher" VERBATIM
    )
    if(NOT CMAKE_GENERATOR MATCHES "Xcode") # In Xcode, libEngine.dylib is embedded automatically.
        add_custom_command(TARGET Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:Engine_DYN>" "$<TARGET_BUNDLE_CONTENT_DIR:Editor>/Frameworks/libEngine.dylib"
        )
    endif()

    # Set up relative path to libEngine.dylib
    add_custom_command (TARGET Editor
        POST_BUILD COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
            "-change" "@rpath/libEngine.dylib" "@executable_path/../Frameworks/libEngine.dylib"
            "$<TARGET_FILE:Editor>" VERBATIM)
endif()

# IDE project organization
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "src"
    FILES ${EDITOR_SRC} ${EDITOR_SRC_WIN32} ${EDITOR_SRC_MACOS}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/content"
    PREFIX "content"
    FILES ${EDITOR_CONTENT}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    PREFIX "resources"
    FILES ${EDITOR_RESOURCES_WIN32} ${EDITOR_RESOURCES_MACOS}
)

# Debugger setup
set_target_properties(Editor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    VS_DEBUGGER_COMMAND_ARGUMENTS "-c \"Editor/content\" -d \"data/Editor\" -e \"Engine/include_glsl\""
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    XCODE_SCHEME_ARGUMENTS "-c \"Editor/content\";-d \"data/Editor\";-e \"Engine/include_glsl\""
)

# Install locations
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS Editor Launcher DESTINATION "Enterprise")
    install(TARGETS Engine_DYN
        RUNTIME DESTINATION "Enterprise" COMPONENT Runtime
        LIBRARY DESTINATION "Enterprise" COMPONENT Runtime
    )

    # Developer resources
    install(TARGETS Engine_LIB DESTINATION "Enterprise/developer")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/Engine/include_glsl" "${CMAKE_CURRENT_SOURCE_DIR}/content" DESTINATION "Enterprise")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/Engine/include" DESTINATION "Enterprise/developer")
    install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.txt" DESTINATION "Enterprise/developer/licenses" RENAME "Enterprise.txt")
    
    # glm
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/Engine/vendor/glm/glm" DESTINATION "Enterprise/developer/include")
    install(FILES "${CMAKE_SOURCE_DIR}/Engine/vendor/glm/copying.txt" DESTINATION "Enterprise/developer/licenses" RENAME "glm.txt")

    # yaml-cpp
    install(TARGETS yaml-cpp DESTINATION "Enterprise/developer")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/Engine/vendor/yaml-cpp/include" DESTINATION "Enterprise/developer")
    install(FILES "${CMAKE_SOURCE_DIR}/Engine/vendor/yaml-cpp/LICENSE" DESTINATION "Enterprise/developer/licenses" RENAME "yaml-cpp.txt")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS Editor DESTINATION ".")
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/Enterprise.app\" \"\" \"\")
        "
    )
endif()
