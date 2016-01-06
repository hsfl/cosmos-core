import qbs

Product {
    type: "staticlibrary"
    name: "CosmosDeviceDisk"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory]
    }

    Depends { name: "support" }

}
