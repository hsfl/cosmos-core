import qbs

Product {

    condition: qbs.targetOS.contains("windows") && qbs.toolchain.contains("msvc")

    type: "staticlibrary"
    name: "dirent"
    files: [ "*.c", "*.h" ]

    Depends { name: "cpp" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "7.0"
    }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory]
    }
}
