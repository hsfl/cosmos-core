# Coding style {#tutorial-coding-style}

This is an overview of the coding conventions when contributing to the COSMOS code base. 
The style has been created using commonly used programming styles and through collaboration of the developers in a way that makes it more efficient to write and visualize code.

Indentation
use 4 spaces for indentation, do not use tabs!

File Names
* use lower case file names (unix style, Qt also uses this naming convention for C++ classes et. al. ). Example: mainwindow.cpp will contain class MainWindow
* for header files just use the extension .h, not .hpp 

Doxygen documentation
* use the Qt style for commenting the code [1]

```
/*!
 * ... text ...
 */
```

* don't add doxygen comments inside the functions since it is not supported. Just add regular comments such as // or /* */

references:
[1] https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html