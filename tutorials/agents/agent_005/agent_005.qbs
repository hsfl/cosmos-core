import qbs

CppApplication {

    type: "application" // To suppress bundle generation on Mac
    consoleApplication: true
    files: "agent_001.cpp"

    Depends { name: "zlib" }
    Depends { name: "CosmosSupport" }
    Depends { name: "CosmosMath" }
    Depends { name: "CosmosDeviceCpu" }
    Depends { name: "CosmosAgent" }

    Depends {
        condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
        name: "dirent"
    }


    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"
    //cpp.cppFlags: "-std=c++11"


    cpp.includePaths : [
        '../../../libraries/',
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

