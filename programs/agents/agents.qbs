import qbs

Product {
    name : "agent"
    type: "application" // To suppress bundle generation on Mac
    consoleApplication: true
    files: "agent.cpp"

    Depends { name: "zlib" }
    Depends { name: "support" }
    Depends { name: "math" }
    Depends { name: "device-cpu" }
    Depends { name: "CosmosDeviceDisk" }
    Depends { name: "CosmosDeviceVn100" }
    Depends { name: "CosmosDeviceGeneral" }

    Depends {
        condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
        name: "dirent"
    }

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"
    //cpp.cppFlags: "-std=c++11"


    cpp.includePaths : [
        '../../support/',
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


//Product {

//    name : "test"
//    type: "application" // To suppress bundle generation on Mac
//    consoleApplication: true
//    files: "agent1.cpp"

//}

