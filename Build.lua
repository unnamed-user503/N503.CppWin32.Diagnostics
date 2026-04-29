project "N503.CppWin32.Diagnostics"
    kind          "SharedLib" -- ConsoleApp WindowedApp SharedLib StaticLib
    language      "C++"
    cppdialect    "C++20"
    staticruntime "off"
    characterset  "Unicode"

    pchheader "Pch.hpp"
    pchsource "Source/Pch.cpp"

    targetdir  ("%{wks.location}/Binaries/" .. OutputDir .. "/")
    objdir     ("%{wks.location}/Binaries-Intermediates/" .. OutputDir .. "/%{prj.name}")

    vsprops
    {
        -- VcpkgTriplet = "x64-windows-static",
        -- VcpkgEnabled = "true",
        PublicIncludeDirectories = "Include",
    }

    nuget
    {
        "Microsoft.Windows.ImplementationLibrary:1.0.260126.7",
    }

    defines
    {
        "N503_DLL_EXPORTS",
    }

    files
    {
        "Include/**.hpp", "Source/**.cpp", "Source/**.hpp",
    }

    includedirs
    {
        "Include", "Source",
    }

    links
    {
        "N503.CppWin32.Abi",
    }

    prebuildcommands
    {
    }

    postbuildcommands
    {
    }
