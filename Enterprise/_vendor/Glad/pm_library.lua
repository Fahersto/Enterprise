--[[
    Glad library
    Simple library wrapper for Glad project for use in Enterprise.
    Configuration: OpenGL 4.1, Core Profile, C/C++, no extensions
]]

local configurationName = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Glad"
    kind "StaticLib"
    language "C"

    -- Platform-specific build settings
    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
    filter "system:macosx"
        systemversion "10.15"
    filter {}

    -- Build locations
    targetdir ("bin/" .. configurationName .. "/%{prj.name}")
    objdir ("bin-int/" .. configurationName .. "/%{prj.name}")

    -- Configurations are set in the Enterprise solution/workspace premake file.

    -- Platforms
    filter "platforms:Static64"
        architecture "x86_64"
    filter {}
    
        -- Files/Includes/Defines
    files { "include/glad/glad.h", "include/KHR/khrplatform.h", "src/glad.c" }
    includedirs "include"
