import qbs

Product {
    type: "staticlibrary"
    name: "CosmosDeviceCpu"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory]
    }

    Depends { name: "CosmosSupport" }

}
