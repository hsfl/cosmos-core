import qbs

Product  {
    type: "staticlibrary"
    name: "CosmosDeviceGeneral"
    files: [ "*.cpp", "*.h" ]

    cpp.includePaths : [
        '../../support/',
        '../../thirdparty/',
    ]


    Depends { name: "support" }
    Depends { name: "math" }
    Depends { name: "cpp" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory , "../"]
        cpp.defines: ["USING_" + product.name.toUpperCase()]
    }
}
