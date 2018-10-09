solution "skeletal-animation"
configurations { "Debug", "Release" }
configuration "Debug"
    targetdir "../bin/Debug"
configuration "Release"
    targetdir "../bin/Release"

project "skeletal-animation"
    kind "ConsoleApp"
    language "C++"
    linkoptions {
        "-L/usr/local/lib",
        "-L../lib",
        "-lboost_system",
        "-lboost_filesystem",
        "-lglfw",
        "-lassimp",
        "-lglad",
        "-ldl",
    }
    buildoptions { "-std=c++14" }

    includedirs { "../prerequisite", "/usr/local/include", "../skeletal-animation/include" }
    files { "../skeletal-animation/**.h", "../skeletal-animation/**.cpp" }
