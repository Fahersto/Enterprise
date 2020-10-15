--[[
    Enterprise Engine Development Premake Script
    
    Utilized by Premake to generate project and solution/workspace files for
    Enterprise engine development.  To build these, run the appropriate script
    for your platform in "_scripts".
]]

local EP_ProjectName = "Sandbox" -- The name of your Enterprise project.
local configurationName = "%{cfg.buildcfg}-%{cfg.system}"

workspace "Enterprise_Dev"
    startproject (EP_ProjectName)
    configurations { "Debug", "Release", "Dist" }
    platforms { "x64" }

-- Vendor library projects
group "Dependencies"
    include "Enterprise/_vendor/HotConsts/pm_library.lua" -- Hot Constants!
    configmap {
        ["Dist"] = "Release",
        ["x64"] = "Static64"
    }
group ""

-- Engine library
project "Enterprise"
    kind "StaticLib"
    location "%{prj.name}"
    links "HotConsts"
    language "C++"
    cppdialect "C++17"

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
        defines { "EP_CONFIG_DEBUG", "HOTCONSTS_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "EP_CONFIG_RELEASE", "HOTCONSTS_DEBUG" }
        optimize "On"
    filter "configurations:Dist"
        defines "EP_CONFIG_DIST"
        optimize "On"
    filter {}

    -- Platforms (Enterprise is 64-bit only)
    architecture "x86_64"

    -- Files/Includes/Defines
    files {
        -- C/C++ source
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",

        -- Obj-C source
        "%{prj.name}/src/**.m",
        "%{prj.name}/src/**.mm",
    }
    filter { "system:windows", "files:**.m or files:**.mm" }
        buildaction "None"  -- Don't build Obj-C files in MSVC
    filter {}
    sysincludedirs {
        "Enterprise/_vendor/spdlog/include",
        "Enterprise/_vendor/cxx-prettyprint",
        "Enterprise/_vendor/HotConsts/include"
    }
    includedirs "%{prj.name}/src"
    defines "EP_SCOPE_CORE"

    -- Precompiled header
    pchheader "EP_PCH.h"
    pchsource "Enterprise/src/EP_PCH.cpp"
    filter "system:macosx"
        pchheader "src/EP_PCH.h"
    filter {}

-- Game project
project (EP_ProjectName)
    kind "WindowedApp"
    location (EP_ProjectName)
    links "Enterprise"
    language "C++"
    cppdialect "C++17"

    -- Platform-specific build settings
    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
    filter "system:macosx"
        systemversion "10.15"
        links { "Foundation.framework", "AppKit.framework", "Cocoa.framework" }
        xcodebuildsettings {
            ["INFOPLIST_FILE"] = "_resources/macOS/Info.plist",
            -- ["CODE_SIGN_ENTITLEMENTS"] = "_resources/macOS/Enterprise.entitlements" -- This should probably be added per project.
            -- ["ASSETCATALOG_COMPILER_APPICON_NAME"] = "AppIcon" -- Pending an icon build system.
        }
    filter {}

    -- Build locations
    targetdir ("bin/" .. configurationName .. "/%{prj.name}")
    objdir ("bin-int/" .. configurationName .. "/%{prj.name}")

    -- Configurations
    filter "configurations:Debug"
        defines { "EP_CONFIG_DEBUG", "HOTCONSTS_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "EP_CONFIG_RELEASE", "HOTCONSTS_DEBUG" }
        optimize "On"
    filter "configurations:Dist"
        defines "EP_CONFIG_DIST"
        optimize "On"
    filter {}

    -- Platforms (Enterprise is 64-bit only)
    architecture "x86_64"

    -- Files/Includes/Defines
    files {
        -- C/C++ source
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        EP_ProjectName .. "/_resources/**"
    }
    removefiles "**.DS_Store" -- Exclude macOS hidden files
    vpaths { ["src/Resources/*"] = (EP_ProjectName .. "/_resources/**") } -- Nesting "_resources" makes "src" the top level.
    sysincludedirs {
        "Enterprise/src",
        "Enterprise/_vendor/spdlog/include",
        "Enterprise/_vendor/cxx-prettyprint",
        "Enterprise/_vendor/HotConsts/include"
    }
    includedirs "%{prj.name}/src"
    defines "EP_SCOPE_CLIENT"
