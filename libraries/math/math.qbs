import qbs

Product {
    type: "staticlibrary"
    name: "math"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [ ".", "../"]
    }

    cpp.includePaths : [
        '../',
        '../support/',
        '../math/'
    ]


}
