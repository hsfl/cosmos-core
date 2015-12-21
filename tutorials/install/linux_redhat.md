Install COSMOS on Linux RedHat {#install-linux-redhat}
==============================

# System Requirements and User Prerequisites
The general conditions required for operation, and the desirable requirements for best performance are detailed in the following sections.

## Operating Systems
The COSMOS system has been ported to all three major Operating Systems; Linux, MacOSX and Windows 7; including variants within these. As a Graphical User Interface application, MOST will only work under Linux if an X11 server is available.

The COSMOS Tools are built using the Qt development environment, and so is basically a 32 bit program. It should, however, work in both 32 and 64 bit environments.

## Hardware
While MOST will operate with as little as 512 MiB of RAM, certain features
can be quite memory intensive, which can cause significant slow downs. For best performance, it is recommended that you have at least 2 GiB.

MOST has been run on a single core, 32 bit, 1 GHz machine. However, the best performance will be experienced on a 64 bit multi core machine running at at least 2.5 GHz.

While the MOST program takes up only about 50 MiB of disk, its ancillary files take up another 500 MiB. If you also wish to keep the example data on disk, you will use another 500 MiB, so it would be wise to reserve 2 GiB for the entire installation. 

Finally, MOST was designed with high resolution displays in mind and takes full advantage of a 1980x1200 pixel WUXGA screen. It will provide scroll bars automatically if used on a smaller display, but WUXGA and above is desirable.

# COSMOS Installation in Red Hat Enterprise Linux 6 (RHEL6)
COSMOS has been installed in Ubuntu, CentOS and Scientific Linux and the licensed Red Hat Enterprise Linux 6 (RHEL6). The following are the instructions to configure and install COSMOS in RHEL6.

Installation Requirements:
* GCC 4.6 or above (for C++11 support)
* Qt 4.8 (for dynamic UI support) or above 


## Install and configure RHEL6
It is assumed that a new user account is created with the name "cosmos". Check the Red Hat installation.

```
# uname -a
Linux cosmos 2.6.32-279.el6.x86_64 
1 SMP Wed Jun 13 18:24:36 EDT 2012 x86_64 x86_64 x86_64 GNU/Linux
```

Linux Kernel is 2.6, 64 bit.

Add "cosmos" user to sudoers:
```
# su
# visudo -f /etc/sudoers
```

continued on the file linux-redhat.pdf

TODO: copy from linux-redhat.tex and trasnform to markdown