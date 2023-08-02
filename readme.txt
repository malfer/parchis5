Welcome to Parchis v5.1

Parchís v5.1 is a computer program to play Parchís, a board game very popular
in Spain. My Parchís program allows to play any combination of human beings
and pc-driven players (robots). The first program version dates from 1992,
it was coded with TurboC for the DOS operating system. The version 4, from
2007, was adapted to use the MGRX graphics library and was able to run in all
the platforms MGRX supports (DOS, Linux X11 and Console, W32). After that I
recoded the program in Java and released Parchís4A, an Android app, with
lots of improvements. Parchís v5.1 is a new rewrite in C, using the latest
MGRX+GrGUI library with most of the Parchís4A improvements and some new ideas.
Like the version 4 it runs in all the platforms MGRX supports.

Parchís v5.1 installation instructions
======================================

Requirements:
-------------

  The source files:  parchv51.zip
  This document:     readme.txt
  
Dependencies:
-------------

  Before compiling Parchís v5 you must have MGRX with PNG support instaled in
  your system, at least in his version 1.4.3. Go to www.fgrim.com/mgrx for
  instructions.

A. Unpacking the Parchís v5.1 archive
-------------------------------------

  1) 'cd' to a directory where you want the Parchís v5.1 file tree to be
     created as a subdirectory. Examples are:

        DJGPP : C:\MYPROGS
        W32   : C:\MYPROGS
        Linux : ~/myprogs

  2) Unpack the Parchís v5.1 archive:

        unzip parchv51.zip

     This will create the subdirectory 'parchv51'.

B. Compiling Parchís v5.1
-------------------------

  1) Go to the "parchv51" directory

  2) run 'make -f <your makefile>':

     makefile.dj2 for DOS using the DJGPPv2 compiler
     makefile.w32 for Win32 using the TDM-GCC compiler
     makefile.lnx for Linux/console
     makefile.x11 for Linux/X11

     This process generates the "parchis5" executable ("xparchis5" for the
     X11 version).

     Note for DJGPP/W32 users: Do _not_ use an environment variable `SHELL'
     leading to `bash', e.g. `SHELL=/djgpp/bin/bash.exe'. Do not use MSYS
     either. The DJGPP/W32 Makefiles must be run with the standard
     'command.com'.
     
     Note for W32 users: use "mingw32-make" instead "make". The compilation
     was tested with the 32 bit version of "TDM-GCC" mingw32 distribution.

C. Installation
---------------

    It is not necesary to install, simply put the executable and the "pfiles"
    subdirectory where you want and run it.

License
=======

  Parchís v5,1 is free software, you can redistribute and/or modify it under
  the GPLv3 license. See the "LICENSE" file for details.

  Files on the "pfiles" subdirectory are my own work and I want to release
  them to the public domain.

Enjoy, M.Alvarez <malfer at telefonica.net>
