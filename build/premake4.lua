solution "skinned-animation"
configurations { "Debug", "Release" }
configuration "Debug"
    targetdir "../bin/Debug"
configuration "Release"
    targetdir "../bin/Release"

project "skinned-animation"
    kind "ConsoleApp"
    language "C++"
    linkoptions {
        "-L/usr/local/lib",
        "-L../lib",
        "-lboost_system",
        "-lboost_filesystem",
        "-lglfw",
        "-lassimp",
        "-lglad"
    }
    buildoptions { "-std=c++14" }

    includedirs { "../prerequisite", "/usr/local/include" }
    files { "../skeletal-animation/**.h", "../skeletal-animation/**.cpp" }
