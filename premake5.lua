--[[
    Enterprise Engine Development Premake Script
    Utilized by Premake to generate project and solution files.
    To generate projects and solutions, run the appropriate script in scripts/.
]]

workspace "Enterprise_Dev"
    language "C++"
    cppdialect "C++17"
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

    local configurationName = "%{cfg.buildcfg}-%{cfg.system}" -- -%{cfg.architecture}"
    includedirs "%{prj.name}/src"
    targetdir ("bin/" .. configurationName .. "/%{prj.name}")
    objdir ("bin-int/" .. configurationName .. "/%{prj.name}")
    files {
        -- C/C++ source
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
    }

    filter "system:windows"
        defines "EP_PLATFORM_WINDOWS"
        systemversion "latest"
        staticruntime "On"

    filter "system:macosx"
        defines "EP_PLATFORM_MACOS"
        systemversion "10.15"
        files {
            -- Obj-C source
            "%{prj.name}/src/**.m",
            "%{prj.name}/src/**.mm",
        }

    filter {}

    startproject "Sandbox"

project "Enterprise" -- =============================================
    location "Enterprise"
    kind "StaticLib"
    defines "EP_SCOPE_CORE"

    sysincludedirs {
        "Enterprise/vendor/spdlog/include",
        "Enterprise/vendor/cxx-prettyprint"
    }
    pchheader "EP_PCH.h"
    pchsource "Enterprise/src/EP_PCH.cpp"
 
    filter "system:macosx"
        links { "Foundation.framework", "AppKit.framework", "Cocoa.framework" }
        pchheader "src/EP_PCH.h" -- For some reason this has to be relative to the project

    filter {}

project "Sandbox" -- ================================================
    location "Sandbox"
    kind "WindowedApp"
    defines "EP_SCOPE_CLIENT"

    links "Enterprise"
    sysincludedirs {
        "Enterprise/src",
        "Enterprise/vendor/spdlog/include",
        "Enterprise/vendor/cxx-prettyprint"
    }
    files "_Resources/**"
    removefiles "**.DS_Store" -- macOS hidden files

    -- By moving resources into /src, premake will put the whole thing at the project level.
    vpaths {["src/_Resources/*"] = "_Resources/**"}

    filter "system:macosx"
        xcodebuildsettings {
            ["INFOPLIST_FILE"] = "../_Resources/macOS/Info.plist",
            ["CODE_SIGN_ENTITLEMENTS"] = "../_Resources/macOS/Enterprise.entitlements"
            -- ["ASSETCATALOG_COMPILER_APPICON_NAME"] = "AppIcon" -- Pending an icon build system.
        }
    filter {}