import qbs

Product {
    type: "staticlibrary"
    name: "CosmosAgent"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "zlib" }
    Depends { name: "CosmosDeviceCpu" }
    Depends { name: "CosmosSupport" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "7.0"
    }

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "CosmosSupport" }
        Depends { name: "cpp" }
        cpp.includePaths: [ "./", "../"]
    }

    cpp.includePaths : [
        './',
        '../',
        '../support/',
    ]


}
