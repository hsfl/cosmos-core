import qbs

Project {
    name: "core-all"
    references: [
        "tutorials",
        "programs",
        "libraries",
    ]

}

// to install add
// MacOS
// qbs.installRoot:/Applications/cosmos/
