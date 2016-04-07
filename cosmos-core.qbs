import qbs

Project {
    references: [
        "tutorials/tutorials.qbs",
        "programs/programs.qbs",
        "libraries/libraries.qbs",
    ]

}

// to install add
// MacOS
// qbs.installRoot:/Applications/cosmos/
