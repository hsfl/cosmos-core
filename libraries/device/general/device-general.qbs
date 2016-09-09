import qbs

Product  {
    type: "staticlibrary"
    name: "CosmosDeviceGeneral"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.minimumWindowsVersion: "7.0"
    }
    cpp.includePaths : [
        '../../support/',
        '../../thirdparty/',
    ]
    cpp.cxxLanguageVersion : "c++11"
//    cpp.cxxStandardLibrary : "libc++"
//    cpp.commonCompilerFlags : ["-std=c++11", "-stdlib=libc++"]

    Depends { name: "CosmosSupport" }
    Depends { name: "CosmosMath" }
    Depends { name: "CosmosDeviceCpu" }


    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory , "../"]
        cpp.defines: ["USING_" + product.name.toUpperCase()]
    }
}
