import qbs

Project {

    Product {
        name : "gige_list"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "gige_list.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
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
        name : "gige_snap"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "gige_snap.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
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

        //        Group {
        //         qbs.install: true
        //         qbs.installDir: "/Applications/cosmos/bin"
        //         fileTagsFilter: "application"
        //        }
    }



    Product {
        name : "gige_ffc"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "gige_ffc.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
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
        name : "propagator"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "propagator.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosPhysics" }
//        Depends { name: "CosmosDeviceGeneral" }

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
        name : "fast_propagator"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: "fast_propagator.cpp"

        Depends { name: "zlib" }
        Depends { name: "CosmosSupport" }
        Depends { name: "CosmosAgent" }
        Depends { name: "CosmosMath" }
        Depends { name: "CosmosPhysics" }
//        Depends { name: "CosmosDeviceGeneral" }

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
