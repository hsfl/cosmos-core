# Install COSMOS on Linux Ubuntu on Windows 10

Windows 10 allows you to run Ubuntu on the command line, nativelly!
While this is somewhat experimental it can be a great way to simplify your development process if you have a Windows 10 computer.

To read more about the Bash on ubuntu on Windows go to
https://msdn.microsoft.com/en-us/commandline/wsl/about

Follow ths instructions on the insatatllation guide
https://msdn.microsoft.com/en-us/commandline/wsl/install_guide

Once you have completed the installation you can start your bash environment. Once logged in install the essential developer tools such as git, cmake, c++, etc.

To do this please 

```
sudo apt-get update && sudo apt-get install build-essential git cmake
```

next follow the steps to install COSMOS on linux from 
https://bitbucket.org/cosmos/installer-linux-mac


