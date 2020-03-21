--[[
    Enterprise Engine Development Premake Script

    To use this script, run the command for your platform from this folder location.

    Windows (Visual Studio):
    vendor/bin/premake/premake5.exe vs2019

    MacOS (XCode, to be implemented with macOS support):
    vendor/bin/premake/premake5.exe xcode4
]]

workspace "Enterprise_Dev"
    architecture "x64"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Enterprise"
    location "Enterprise"
    kind "StaticLib"
    language "C++"

    -- Build locations
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Set up PCH
    pchheader "EP_PCH.h"
    pchsource "Enterprise/src/EP_PCH.cpp"

    -- Used to write library code which behaves differently in client
    defines "EP_SCOPE_CORE"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{prj.name}/vendor/cxx-prettyprint"
    }
 
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        defines "EP_PLATFORM_WINDOWS"

    -- To be implemented with macOS support
    -- filter "system:macosx"
    --     ???
    --     defines "EP_PLATFORM_MACOS"

    filter "configurations:Debug"
        defines "EP_CONFIG_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "EP_CONFIG_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "EP_CONFIG_DIST"
        optimize "On"


project "Sandbox"
    location "Sandbox"
    kind "WindowedApp"
    language "C++"

    -- Build locations
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Used to write library code which behaves differently in client
    defines "EP_SCOPE_CLIENT"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Enterprise/src",
        "Enterprise/vendor/spdlog/include",
        "Enterprise/vendor/cxx-prettyprint"
    }

    links "Enterprise"
 
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        defines "EP_PLATFORM_WINDOWS"

    -- To be implemented with macOS support
    -- filter "system:macosx"
    --     ???
    --     defines "EP_PLATFORM_MACOS"

    filter "configurations:Debug"
        defines "EP_CONFIG_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "EP_CONFIG_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "EP_CONFIG_DIST"
        optimize "On"