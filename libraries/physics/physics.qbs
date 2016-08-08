import qbs

Product {
    type: "staticlibrary"
    name: "CosmosPhysics"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "7.0"
    }
    cpp.cxxLanguageVersion : "c++11"

    Depends { name: "zlib" }

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
