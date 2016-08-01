import qbs

Product  { // could be DynamicLibrary but at this point loading a dll does not seem to work well
    type: "staticlibrary"
    name: "CosmosSupport"

    files: [
        "*.cpp",
        "*.h",
    ]

    Depends { name: "cpp" }
    Depends { name: "CosmosMath" }
    Depends { name: "zlib" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "7.0"
    }
    cpp.cxxLanguageVersion : "c++11"
    cpp.commonCompilerFlags : "-std=c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: ["./"]
    }

    cpp.includePaths : [
        '.',
        '../',
        '../thirdparty/',
    ]

}
