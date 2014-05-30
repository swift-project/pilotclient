# Copyright (C) 2013 VATSIM Community / authors
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/. */

### Version

BLACK_VER_MAJ     =   0
BLACK_VER_MIN     =   4
BLACK_VER_PAT     =   0
BLACK_VERSION     =   $${BLACK_VER_MAJ}.$${BLACK_VER_MIN}.$${BLACK_VER_PAT}
DEFINES += BLACK_VERSION=$$BLACK_VERSION

###########################
# Install paths
###########################


###########################
# Build configuration
###########################

BLACK_CONFIG    += BlackCore
BLACK_CONFIG    += BlackGui
BLACK_CONFIG    += BlackSound
BLACK_CONFIG	+= BlackSim
BLACK_CONFIG    += Samples
BLACK_CONFIG    += Unittests
BLACK_CONFIG    += FSX
#BLACK_CONFIG    += XPlane
#BLACK_CONFIG    += Doxygen

################################
# Defines, for compile time code
################################

contains(BLACK_CONFIG, BlackSound) { DEFINES += WITH_BLACKSOUND }
contains(BLACK_CONFIG, BlackSim) { DEFINES += WITH_BLACKSIM }
contains(BLACK_CONFIG, BlackCore) { DEFINES += WITH_BLACKCORE }
contains(BLACK_CONFIG, BlackGui) { DEFINES += WITH_BLACKGUI }
contains(BLACK_CONFIG, FSX) { DEFINES += WITH_FSX }
contains(BLACK_CONFIG, XPlane) { DEFINES += WITH_XPLANE }
