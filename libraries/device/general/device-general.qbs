import qbs

Product  {
    type: "staticlibrary"
    name: "CosmosDeviceGeneral"
    files: [ "*.cpp", "*.h" ]

    Depends { name: "cpp" }
    cpp.includePaths : [
        '../../support/',
        '../../thirdparty/',
    ]
//    cpp.cxxStandardLibrary : "libc++"
    cpp.commonCompilerFlags : ["-std=c++11", "-stdlib=libc++"]


    Depends { name: "CosmosSupport" }
    Depends { name: "CosmosMath" }


    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory , "../"]
        cpp.defines: ["USING_" + product.name.toUpperCase()]
    }
}
