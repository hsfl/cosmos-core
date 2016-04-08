import qbs

Product {
    type: "staticlibrary"
    name: "CosmosMath"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [ "./", "../"]
    }

    cpp.includePaths : [
        './',
        '../',
        '../support/',
    ]


}
