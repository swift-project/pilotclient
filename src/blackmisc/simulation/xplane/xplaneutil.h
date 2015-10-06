/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_XPLANE_XPLANEUTIL_H
#define BLACKMISC_SIMULATION_XPLANE_XPLANEUTIL_H

#include "blackmisc/blackmiscexport.h"
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
        {
            //! XPlane utils
            class BLACKMISC_EXPORT CXPlaneUtil
            {
            public:
                //! Constructor
                CXPlaneUtil() = delete;

                //! XPlane 9 directory
                //! \todo Test on OSX
                static QString xplane9Dir();

                //! XPlane 10 directory
                //! \todo Test on OSX
                static QString xplane10Dir();

                //! XBus legacy directory
                static QString xbusLegacyDir();
            };

        } // namespace
    } // namespace
} // namespace

#endif // guard
