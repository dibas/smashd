# smashd
This repo contains a poc for sd card access on the Wii U.  It is untested and therefore **likely broken**. Also, additional process permissions may be needed to successfully execute this code.

**What you need**
The [libwiiu repo](https://github.com/wiiudev/libwiiu) cloned somewhere on your computer.
export LIBWIIU=pathtolibwiiurepo (see the Makefile for details)
Adjust the include's in loader.h

**Compilation**
Just run the following script in the libwiiu repo folder

    python build.py /path/to/smashd_repo