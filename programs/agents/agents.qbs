import qbs

Project {

    Product {
        name : "agent"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "agent.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosDeviceCpu" }
        Depends { name: "CosmosDeviceDisk" }
        Depends { name: "CosmosDeviceVn100" }
        Depends { name: "CosmosDeviceGeneral" }

        Depends {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
            name: "dirent"
        }

        Depends { name: "cpp" }
        Properties {
            condition: qbs.targetOS.contains("windows")
            cpp.minimumWindowsVersion: "6.0"
        }
        cpp.cxxLanguageVersion : "c++11"
        cpp.staticLibraries: ["wsock32", "winmm", "ws2_32", "iphlpapi"]


        cpp.includePaths : [
            '../../support/',
            '../../../libraries/',
            '../../../libraries/thirdparty/']


        //        // define the libraries to use with MINGW
        //        Properties {
        //            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("mingw")
        //            cpp.dynamicLibraries: ["pthread", "wsock32", "winmm", "ws2_32", "iphlpapi"]
        //            cpp.minimumWindowsVersion: "7.0"
        //        }

        //        // define the libraries to use with MSVC
        //        Properties {
        //            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
        //            cpp.dynamicLibraries: ["wsock32", "winmm", "ws2_32", "iphlpapi"]
        //        }

        Group {
            qbs.install: true
            qbs.installDir: "bin"
            fileTagsFilter: "application"
        }
    }


    Product {
        name : "agent_cpu"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "agent_cpu.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosDeviceCpu" }
        Depends { name: "CosmosDeviceDisk" }
        Depends { name: "CosmosDeviceGeneral" }

        Depends {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
            name: "dirent"
        }

        Depends { name: "cpp" }
        cpp.cxxLanguageVersion : "c++11"

        cpp.includePaths : [
            '../../support/',
            '../../../libraries/',
            '../../../libraries/thirdparty/']

        //        Group {
        //         qbs.install: true
        //         qbs.installDir: "/Applications/cosmos/bin"
        //         fileTagsFilter: "application"
        //        }
    }



    Product {
        name : "agent_data"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "agent_data.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosDeviceCpu" }
        Depends { name: "CosmosDeviceDisk" }
        Depends { name: "CosmosDeviceGeneral" }

        Depends {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
            name: "dirent"
        }

        Depends { name: "cpp" }
        cpp.cxxLanguageVersion : "c++11"

//        cpp.dynamicLibraries: ["wsock32", "winmm", "ws2_32", "iphlpapi"]
        cpp.staticLibraries: ["wsock32", "winmm", "ws2_32", "iphlpapi"]

        cpp.includePaths : [
            '../../support/',
            '../../../libraries/',
            '../../../libraries/thirdparty/']

        Group {
            qbs.install: true
            qbs.installDir: "bin"
            fileTagsFilter: "application"
        }
    }


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

