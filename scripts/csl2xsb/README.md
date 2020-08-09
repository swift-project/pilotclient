# CSL2XSB
Converts CSL packages to the XSB format for use in [LiveTraffic](https://twinfan.gitbook.io/livetraffic/) or more specifically: as expected by
the [XPMP2 library](https://github.com/TwinFan/XPMP2), a success to the
classic libxplanemp.

`CSL2XSB` updates some CSL dataRefs (engine/prop rotation, reversers) so they become available to LiveTraffic (and other XPMP2-driven plugins).

Tested with the following CSL model providers:

- [Bluebell Package](https://forums.x-plane.org/index.php?/files/file/37041-bluebell-obj8-csl-packages/)
- [X-CSL](https://csl.x-air.ru/?lang_id=43)

As this is a Python 3 script you [need Python 3](https://www.python.org/downloads/).
Tested with Python 3.7.3.

## Simple usage in Windows

- [Download](https://www.python.org/downloads/) and install Python 3 using the latest "Windows x86-64 web-based installer"
    - Important: Check (select) the option "Add Python 3.x to PATH" at the bottom of the "Install Python" window.
    - Click on "Install Now". Python will install.
    - When done, click "Close" in the "Setup was successfull" screen. Now you've got Python 3.
- Download a CSL package like from X-CSL.
- Make a copy of it!
- Put the `CSL2XSB.py` script into the base directory of the that copy.
- Double-lick the `CSL2XSB.py` script in the explorer to start it. It will ask you if you want to run the script in that current directory. Enter "y" and hit Enter.

## Synopsis

```
usage: CSL2XSB.py [-h] [--noupdate] [--norecursion] [-v] [--replaceDR TEXT]
                  [path]

CSL2XSB 0.3.1: Convert CSL packages to XPMP2 format, convert some animation
dataRefs. Tested with: Bluebell, X-CSL.

positional arguments:
  path              Base path, searched recursively for CSL packages
                    identified by existing xsb_aircraft.txt files

optional arguments:
  -h, --help        show this help message and exit
  --noupdate        Suppress update of OBJ8 files if there are no additional
                    textures
  --norecursion     Do not search directories recursively
  -v, --verbose     More detailed output about every change
  --replaceDR TEXT  Replace dataRef's root 'libxplanemp' with TEXT. CAUTION:
                    This works with LiveTraffic up to v1.5 only. CSLs'
                    animations/lights will no longer work with standard
                    multipayer clients nor with LiveTraffic starting from
                    v2.0!
```

This will likely produce many new files, especially new `.OBJ` files, so disk usage increases.

## Background

The format of CSL packages has originally been defined with the creation of the
[libxplanemp library](https://github.com/kuroneko/libxplanemp/wiki).
Since then, various dialects evolved, e.g. in X-IvAp or PilotEdge or the like.
That means that CSL packages created for these clients cannot to their
full extend be used in LiveTraffic, which uses the original format.
There are disputes about how future-proof each format is.

To make other packages accessible to LiveTraffic (and likely, though not tested:
XSquawkBox) this Python script `CSL2XSB.py` converts their format.

This only works for the - nowadays most common - OBJ8 format, which is the only
format supported by XPMP2ever.

- `xsb_aircraft.txt` file:
  - Validates entries in `xsb_aircraft.txt` file and corrects some common errors
  - Warns about non-existing `.obj` or texture files, which will cause error
    when actually using that model.
  - Comments out models of unsupported format (`.acf` or OBJ7)

- `.obj` files referred to by `xsb_aircraft.txt` file:
  - Rewrites OBJ8 models for which texture have been defined in additional
    parameters to the `OBJ8` command, which cannot be supported in XPMP2 and
    X-Plane's instancing.
  - Replaces a number of older dataRef names (dating back to WT times) with
    those now offered by XPMP2, so that additional animations like props, rotors,
    or reversers become accessible.
  - Removes unsupported commands `TEXTURE`, `HASGEAR`, `OFFSET`.

## Package-specific Information

### X-CSL

X-CSL packages can be downloaded [here](https://csl.x-air.ru/downloads?lang_id=43).
If you don't already have the package (e.g. because you use X-IvAp)
then download and start the installer. The installer will _not_ identify
LiveTraffic as a supported plugin. Instead, from the menu select
`File > Select Custom Path` and specify a path where the CSL packages are
to be downloaded to and where they later can be updated.

Do not let `CSL2XSB.py` run on this original download. Always make a copy of
the entire folder into a folder LiveTraffic can reach, e.g. to
`<...>/LiveTraffic/Resources/X-CSL`. Now run the script on this copy,
e.g. like this:
```
python CSL2XSB.py <...>/LiveTraffic/Resources/X-CSL
```
(Note that in some environments like Mac OS you need to specifically call
`python3` instead of just `python`.)

You can always repeat the above call and the script shall do it just again
(e.g. in case you modified any files manually). It keeps copies of original
files that it needs for a repeated run.

What the script then does is, in brief, as follows:
1. It searches for `xsb_aircraft.txt` files. If it does not find any in the
   current directory it will recursively dig deeper into the folder structure.
   So it will eventually find all folders below `X-CSL`.
2. It copies the `xsb_aircraft.txt` file to `xsb_aircraft.txt.orig` and
   reads that to create a new `xsb_aircraft.txt` file.
3. **The `OBJ8 SOLID/LIGHTS/GLASS` lines are at the core:**
   Here, additional parameters often define the texture files to use.
   The original format does not support these texture parameters.
   Instead, the textures are to be defined in the `.OBJ` file.
    - To remedy this, the script now also reads the `.OBJ` file and
      writes a _new_ version of it replacing the `TEXTURE` and `TEXTURE_LIT` lines.
    - This new `.OBJ` file is then referred to in the `OBJ8 SOLID/LIGHTS` line
      in the output version of `xsb_aircraft.txt`.
      (An original `OBJ8 GLASS` line will be written to output as `OBJ8 SOLID` as `GLASS` is now deprecated.)
    - The availability of the referred texture and lit-texture files is tested.
      Some of them do not exist in the package, which causes warnings by the script.
      This is a problem in the original X-CSL package. In a few cases,
      the script can find a replacement by just replacing the extension of the texture file.
4. Minor other changes:
    - Replace the non-existing ICAO aircraft designator `MD80` with `MD81`.
    - Remove deprecated lines like `LOW_LOD` from `xsb_aircraft.txt`
    - Replace `:` or spaces in `OBJ8` aircraft names with `_`.

The size of the complete X-CSL package increases from about 2 GB to about
3.2 GB due to the additionally created `.OBJ` files.

The resulting folder structures and its files should be usable by LiveTraffic
and produce no more warnings in `Log.txt`.

See LiveTraffic's [documentation on CSL settings](https://twinfan.gitbook.io/livetraffic/setup/configuration/settings-csl) for how to provide LiveTraffic with the path to the converted X-CSL package.

### Bluebell

The Bluebell package is the standard package recommended for usage with LiveTraffic.
Many CSL objects in the Bluebell package are capable of turning rotors or
open reversers. But as there was no `libxplanemp` CSL dataRef to control
these animation they stayed unchanged in the `.obj` files when the Bluebell
package was originally created, e.g. like `cjs/world_traffic/engine_rotation_angle1`.

XPMP2 (used by LiveTraffic) now implements more CSL dataRefs than in the
standard `libxplanemp` version, e.g. for engine/prop rotation and
reversers animation, and tries to stick to a
[standard set by PilotEdge](https://www.pilotedge.net/pages/csl-authoring)
as far as possible.

`CSL2XSB` replaces the unchanged dataRefs with the ones LiveTraffic now supports
so that rotors do rotate etc. The example above will be replaced with
`libxplanemp/engines/engine_rotation_angle_deg`.
For a complete list of replacement dataRefs see the very beginning of the script
in the map called `_DR`.
