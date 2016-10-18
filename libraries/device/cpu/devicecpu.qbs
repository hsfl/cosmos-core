import qbs

Product {
    type: "staticlibrary"
    name: "CosmosDeviceCpu"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "7.0"
    }
    cpp.cxxLanguageVersion : "c++11"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory, "../../device/cpu/"]
    }

//    Depends { name: "CosmosSupport" }
    cpp.includePaths: ["../../"]

}
