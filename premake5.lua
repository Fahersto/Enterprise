--[[
    Enterprise Engine Development Premake Script
    
    Utilized by Premake to generate project and solution/workspace files for
    Enterprise engine development.  To build these, run the appropriate script
    for your platform in "_scripts".
]]

-- The name of your Enterprise project.
local EP_ProjectName = "Sandbox"
-- Your organization identifier (used in macOS app bundle)
local APPLE_OrganizationIdentifier = "com.MichaelMartz"

local configurationName = "%{cfg.buildcfg}-%{cfg.system}"

workspace "Enterprise_Dev"
    startproject (EP_ProjectName)
    configurations { "Debug", "Dev", "Dist" }
    platforms { "x64" }

-- Vendor library projects
group "Dependencies"
    -- Instead of mapping the solution/workspace's configurations with configmap,
    -- we define new ones for each project.  This is required because XCode does
    -- not support subprojects with differing configuration names.

    include "Enterprise/_vendor/CTSpookyHash/pm_library.lua" -- CTSpookyHash
        filter "configurations:Dev or Dist"
            optimize "On"
        filter {}
        
    include "Enterprise/_vendor/HotConsts/pm_library.lua" -- Hot Constants!
        filter "configurations:Dev or Dist"
            optimize "On"
        filter {}
group ""

-- Engine library
project "Enterprise"
    kind "StaticLib"
    location "%{prj.name}"
    links { "CTSpookyHash", "HotConsts" }
    language "C++"

    -- Platform-specific build settings
    filter "system:windows"
        systemversion "latest"
        buildoptions{ "/std:c++latest" } -- C++20 Draft
        staticruntime "On"
    filter "system:macosx"
        systemversion "10.15"
        buildoptions{ "-std=c++2a" } -- C++20 Draft
    filter {}

    -- Build locations
    targetdir ("bin/" .. configurationName .. "/%{prj.name}")
    objdir ("bin-int/" .. configurationName .. "/%{prj.name}")

    -- Configurations
    filter "configurations:Debug"
        defines { "EP_CONFIG_DEBUG", "HOTCONSTS_DEBUG" }
        symbols "On"
    filter "configurations:Dev"
        defines { "EP_CONFIG_DEV", "HOTCONSTS_DEBUG" }
        optimize "On"
    filter "configurations:Dist"
        defines "EP_CONFIG_DIST"
        optimize "On"
    filter {}

    -- Platforms (Enterprise is 64-bit only)
    architecture "x86_64"

    -- Files/Includes/Defines
    files 
    {
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
    sysincludedirs 
    {
        "Enterprise/_vendor/spdlog/include",
        "Enterprise/_vendor/cxx-prettyprint",
        "Enterprise/_vendor/CTSpookyHash/include",
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

    -- Platform-specific build settings
    filter "system:windows"
        systemversion "latest"
        buildoptions{ "/std:c++latest" } -- C++20 Draft
        links { "xinput" }
        staticruntime "On"
    filter "system:macosx"
        systemversion "10.15"
        buildoptions{ "-std=c++2a" } -- C++20 Draft
        links { "AppKit.framework", "GameController.framework" }
        postbuildcommands
        {
            "mkdir -p Content",
            "mkdir -p ../bin/" .. configurationName .. "/%{prj.name}/%{prj.name}.app/Contents/Resources/Content",
            "rsync -avu --delete --exclude=\".*\" Content/ ../bin/" .. configurationName .. "/%{prj.name}/%{prj.name}.app/Contents/Resources/Content" 
        }
        xcodebuildsettings
        {
            ["INFOPLIST_FILE"] = "_resources/macOS/Info.plist",
            ["PRODUCT_BUNDLE_IDENTIFIER"] = APPLE_OrganizationIdentifier .. "." .. EP_ProjectName;
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
    filter "configurations:Dev"
        defines { "EP_CONFIG_DEV", "HOTCONSTS_DEBUG" }
        optimize "On"
    filter "configurations:Dist"
        defines "EP_CONFIG_DIST"
        optimize "On"
    filter {}

    -- Platforms (Enterprise is 64-bit only)
    architecture "x86_64"

    -- Files/Includes/Defines
    files
    {
        -- C/C++ source
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }
    removefiles "**.DS_Store" -- Exclude macOS hidden files
    vpaths { ["src/Resources/*"] = (EP_ProjectName .. "/_resources/**") } -- Nesting "_resources" makes "src" the top level.
    sysincludedirs
    {
        "Enterprise/src",
        "Enterprise/_vendor/spdlog/include",
        "Enterprise/_vendor/cxx-prettyprint",
        "Enterprise/_vendor/CTSpookyHash/include",
        "Enterprise/_vendor/HotConsts/include"
    }
    includedirs "%{prj.name}/src"
    defines "EP_SCOPE_CLIENT"
