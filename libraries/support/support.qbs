import qbs

Product  { // could be DynamicLibrary but at this point loading a dll does not seem to work well
    type: "staticlibrary"
    name: "CosmosSupport"

    files: [
        "*.cpp",
        "*.h",
    ]

    Depends { name: "zlib" }
    Depends { name: "CosmosMath" }

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"
//    cpp.cxxStandardLibrary : "libc++" // -stdlib=libc++
    cpp.commonCompilerFlags : "-std=c++0x"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: ["."]
    }

    cpp.includePaths : [
        '.',
        '../',
        '../thirdparty/',
    ]



}
