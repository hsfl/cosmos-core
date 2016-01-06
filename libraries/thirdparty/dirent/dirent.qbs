import qbs

Product {
    type: "staticlibrary"
    name: "dirent"
    files: [ "*.c", "*.h" ]

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory]
    }
}
