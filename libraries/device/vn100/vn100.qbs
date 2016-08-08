import qbs

Product {
    type: "staticlibrary"
    name: "CosmosDeviceVn100"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "CosmosSupport" }
    Depends { name: "CosmosDeviceGeneral" }
    Depends { name: "CosmosMath" }
    Depends { name: "zlib" }

    Depends { name: "cpp" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "7.0"
    }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [ "." ]
    }

}
