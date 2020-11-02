--[[
    SpookyHash Library
    Simple library wrapper for the SpookyHash algorithm by Bob Jenkins for use 
    in Enterprise Engine.
]]

local configurationName = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "SpookyHash"
    kind "StaticLib"
    language "C++"
    cppdialect "C++14"

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

    -- Configurations
    filter "configurations:Debug"
        symbols "On"
    filter "configurations:Release"
        optimize "On"
    filter {}
    
    -- Platforms
    filter "platforms:Static64"
        architecture "x86_64"
    -- filter "platforms:Static32" -- Enterprise is 64-bit only.
    --     architecture "x86"
    filter {}
    
    -- Files/Includes/Defines
    files { "include/SpookyHash/SpookyV2.h", "src/SpookyV2.cpp" }
    includedirs "include/SpookyHash"
