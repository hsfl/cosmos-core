import qbs

CppApplication {
    type: "application" // To suppress bundle generation on Mac
    consoleApplication: true
    files: "agent_002.cpp"


    Depends { name: "zlib" }
    Depends { name: "support" }
    Depends { name: "math" }
    Depends { name: "device-cpu" }


    Group {
        condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
        name: "core-thirdparty-dirent"
        files: [
            "../../../libraries/thirdparty/dirent/*.c",
            "../../../libraries/thirdparty/dirent/*.h"
        ]
    }


    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"
    //cpp.cppFlags: "-std=c++11"


    cpp.includePaths : [
        '../../../libraries/',
        '../../../libraries/support/',
        '../../../libraries/math/',
        '../../../libraries/thirdparty/']

    // define the libraries to use with MINGW
    Properties {
        condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("mingw")
        cpp.dynamicLibraries: ["pthread", "wsock32", "winmm", "ws2_32", "iphlpapi"]
        cpp.minimumWindowsVersion: "7.0"
    }

    // define the libraries to use with MSVC
    Properties {
        condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
        cpp.dynamicLibraries: ["wsock32", "winmm", "ws2_32", "iphlpapi"]
    }
}
