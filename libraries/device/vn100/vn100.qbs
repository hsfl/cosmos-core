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
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [ "." ]
    }

}
