import qbs

Product  { // could be DynamicLibrary but at this point loading a dll does not seem to work well
    type: "staticlibrary"
    name: "support"

    files: [ "*.cpp", "*.h" ]

    Depends { name: "zlib" }
    Depends { name: "math" }

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion : "c++11"


    Export {
        Depends { name: "cpp" }
        cpp.includePaths: ["."]
    }

    cpp.includePaths : [
        '../',
        '../thirdparty/',
    ]



}
