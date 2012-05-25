#libxplanemp
###Multiplayer library for X-Plane

This is libxplanemp, the multiplayer code for X-Plane.  This code is
used in both the XSquawkBox and X-IvAp projects.

See README.MULTIPLAYER for more information.

To clone this repository to your local drive, use:
git clone https://github.com/wadesworld2112/libxplanemp

###Compilation Note

The files CSLLoaderThread.h and CSLLoaderThread.cpp are incomplete and will generate
errors.  They are not currently used.  The library will compile if these 
files are not included.  

Fixes to these files are welcome.

## Note to X-Plane and X-IvAp Developers

This is now the authoratative repository for the mulitplayer code.  If you 
have previously been using Martin Domig's X-IvAp subversion repository on SourceForge,
the multiplayer code contained within that repository is now deprecated and will 
be removed.

Similarly, if you have been using the xivap repository provided by VATSIM, that
repository is now deprecated and will be deleted.

All future updates will be made to this repository.

##Migration Note

The two separate versions of the multiplayer code have been combined into this
repository.  Tags have been added for ease of access, should you
desire to obtain the code in the state it existed at the time of the 
repository merge.

To do so, after cloning the repository, run either of the following commands:

Multiplayer as it existed on X-IvAp SourceForge:
git checkout -b <new branch name> last_xivap_subversion

Multiplayer as it existed on XSB/VATSIM git repository:
git checkout -b <new branch name> last_xivap_xsb

##License

Copyright (c) 2006, Ben Supnik and Chris Serio.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
