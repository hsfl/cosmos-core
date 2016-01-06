import qbs

Project {

    references: [
        "thirdparty/zlib",
        "thirdparty/dirent",
        "support",
        "math",
        "device/cpu",
        "device/disk",
        "device/vn100",
        "device/general"
    ]
}
