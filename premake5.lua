--[[
    Enterprise Engine Development Premake Script
    Utilized by Premake to generate project and solution files.
    To generate projects and solutions, run the appropriate script in "_scripts".
]]

local EP_ProjectName = "Sandbox" -- This string will be the name of your Enterprise project.

workspace "Enterprise_Dev"
    language "C++"
    -- cppdialect "C++20"
    architecture "x86_64"

    configurations { "Debug", "Release", "Dist" }
    filter "configurations:Debug"
        defines "EP_CONFIG_DEBUG"
        symbols "On"
    filter "configurations:Release"
        defines "EP_CONFIG_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        defines "EP_CONFIG_DIST"
        optimize "On"
    filter {}

    local configurationName = "%{cfg.buildcfg}-%{cfg.system}"
    includedirs "%{prj.name}/src"
    targetdir ("bin/" .. configurationName .. "/%{prj.name}")
    objdir ("bin-int/" .. configurationName .. "/%{prj.name}")
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

    filter "system:windows"
        systemversion "latest"
        buildoptions{ "/std:c++latest" }
        staticruntime "On"

    filter "system:macosx"
        systemversion "10.15"
        buildoptions{ "-std=c++2a" }

    filter {}

    startproject (EP_ProjectName)

project "Enterprise" -- =============================================
    location "%{prj.name}"
    kind "StaticLib"
    defines "EP_SCOPE_CORE"

    sysincludedirs {
        "Enterprise/_vendor/spdlog/include",
        "Enterprise/_vendor/cxx-prettyprint"
    }
    pchheader "EP_PCH.h"
    pchsource "Enterprise/src/EP_PCH.cpp"
 
    filter "system:macosx"
        pchheader "src/EP_PCH.h" -- For some reason this has to be relative to the project

    filter {}

project (EP_ProjectName) -- =========================================
    location (EP_ProjectName)
    kind "WindowedApp"
    defines "EP_SCOPE_CLIENT"

    links "Enterprise"
    sysincludedirs {
        "Enterprise/src",
        "Enterprise/_vendor/spdlog/include",
        "Enterprise/_vendor/cxx-prettyprint"
    }
    files (EP_ProjectName .. "/_resources/**")
    removefiles "**.DS_Store" -- Exclude macOS hidden files
    vpaths { ["src/Resources/*"] = (EP_ProjectName .. "/_resources/**") } -- Nesting "_resources" makes "src" the top level.

    filter "system:macosx"
        links { "Foundation.framework", "AppKit.framework", "Cocoa.framework" }
        xcodebuildsettings {
            ["INFOPLIST_FILE"] = "_resources/macOS/Info.plist",
            -- ["CODE_SIGN_ENTITLEMENTS"] = "_resources/macOS/Enterprise.entitlements" -- This should probably be added per project.
            -- ["ASSETCATALOG_COMPILER_APPICON_NAME"] = "AppIcon" -- Pending an icon build system.
        }
    filter {}
