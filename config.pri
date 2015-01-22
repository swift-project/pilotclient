# Copyright (C) 2013
# swift project Community / Contributors
#
# This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
# directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
# including this file, may be copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE file.

### Version

BLACK_VER_MAJ     =   0
BLACK_VER_MIN     =   6
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
BLACK_CONFIG    += BlackInput
BLACK_CONFIG	+= BlackSim
BLACK_CONFIG    += Samples
BLACK_CONFIG    += Unittests
BLACK_CONFIG    += FS9
BLACK_CONFIG    += FSX
BLACK_CONFIG    += XPlane
BLACK_CONFIG    += ProfileRelease
#BLACK_CONFIG    += Doxygen
