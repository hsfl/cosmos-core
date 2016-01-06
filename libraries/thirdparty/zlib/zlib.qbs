import qbs

Product  { // could be DynamicLibrary but at this point loading a dll does not seem to work well
    type: "staticlibrary"
    name: "zlib"
    files: [ "*.c", "*.h" ]

    Depends { name: "cpp" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory , "../"]
        cpp.defines: ["USING_" + product.name.toUpperCase()]
    }
}
