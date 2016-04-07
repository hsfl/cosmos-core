import qbs

Product  { // could be DynamicLibrary but at this point loading a dll does not seem to work well
    type: "staticlibrary"
    name: "CosmosSupport"

    files: [
        "*.cpp",
        "*.h",
    ]

    Depends { name: "cpp" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "6.0"
    }
    cpp.cxxLanguageVersion : "c++11"
//    cpp.cxxStandardLibrary : "libc++" // -stdlib=libc++
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
