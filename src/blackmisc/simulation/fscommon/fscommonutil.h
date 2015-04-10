/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMONUTIL_H
#define BLACKMISC_SIMULATION_FSCOMMONUTIL_H

#include "blackmisc/blackmiscexport.h"
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! FS9/X utils
            class BLACKMISC_EXPORT CFsCommonUtil
            {
            public:
                CFsCommonUtil();

                //! FSX directory obtained from registry
                static QString fsxDirFromRegistry();

                //! FSX's simObject directory from registry
                static QString fsxSimObjectsDirFromRegistry();

            };

        } // namespace
    } // namespace
} // namespace

#endif // guard
