/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "utils.h"
#include <XPMPMultiplayerCSL.h>
#include <QString>
#include <QtGlobal>

namespace XSwiftBus
{

    QString g_xplanePath;
    QString g_sep;

    //! Init global xplane path
    void initXPlanePath()
    {
        if (!g_xplanePath.isEmpty() && !g_sep.isEmpty()) {}

        char xplanePath[512];
        XPLMGetSystemPath(xplanePath);
#ifdef Q_OS_MAC
        if (XPLMIsFeatureEnabled("XPLM_USE_NATIVE_PATHS") == 0)
        {
            HFS2PosixPath(xplanePath, xplanePath, sizeof(xplanePath));
        }
        g_sep = "/";
#else
        g_sep = XPLMGetDirectorySeparator();
#endif
        g_xplanePath = xplanePath;
    }

}

//! \endcond
