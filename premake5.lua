--[[
    Enterprise_dev premake5.lua
    Run a script in the same folder as this file to generate the project files for Enterprise engine development.  The
    following scripts are available as .bat (Windows) and bash (MacOS) files for easy execution.

    Windows (Visual Studio):
    vendor/bin/premake/premake5.exe vs2019

    MacOS (XCode):
    vendor/bin/premake/premake5.exe xcode4
]]

workspace "Enterprise_dev"
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

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    defines "EP_SCOPE_CORE"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include"
    }
 
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        defines "EP_PLATFORM_WINDOWS"

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

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

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
    }

    links
    {
        "Enterprise"
    }
 
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        defines "EP_PLATFORM_WINDOWS"

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