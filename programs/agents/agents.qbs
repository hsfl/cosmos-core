import qbs

Project {

    Product {
        name : "agent"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: [
            "agent.cpp",
        ]

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
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
        name : "agent_client"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: [
            "agent_client.cpp",
        ]

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
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
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosDeviceCpu" }
        Depends { name: "CosmosDeviceDisk" }
        Depends { name: "CosmosDeviceGeneral" }

        Depends {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
            name: "dirent"
        }

        Depends { name: "cpp" }
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
        cpp.cxxLanguageVersion : "c++11"

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



    Product {
        name : "agent_data"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "agent_data.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosDeviceCpu" }
        Depends { name: "CosmosDeviceDisk" }
        Depends { name: "CosmosDeviceGeneral" }

        Depends {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
            name: "dirent"
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

        Depends { name: "cpp" }
        cpp.cxxLanguageVersion : "c++11"

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



    Product {
        name : "agent_arduino"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "agent_arduino.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosDeviceCpu" }
        Depends { name: "CosmosDeviceDisk" }
        Depends { name: "CosmosDeviceGeneral" }

        Depends {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
            name: "dirent"
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

        Depends { name: "cpp" }
        cpp.cxxLanguageVersion : "c++11"

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

    Product {
        name : "agent_forward"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "agent_forward.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }

        Depends {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")
            name: "dirent"
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

        Depends { name: "cpp" }
        cpp.cxxLanguageVersion : "c++11"

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

}
