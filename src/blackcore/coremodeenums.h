/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIMODEENUMS_H
#define BLACKGUI_GUIMODEENUMS_H

#include <QString>

namespace BlackCore
{
    //! Modes, how GUI can be started (core/GUI)
    struct CoreModes
    {
    public:
        //! Core runs how and where?
        enum CoreMode
        {
            CoreInGuiProcess,
            CoreExternal
        };

        //! String to core mode
        static CoreMode stringToCoreMode(const QString &m)
        {
            QString cm(m.toLower().trimmed());
            if (cm.isEmpty()) { return CoreInGuiProcess; }
            if (m == coreModeToString(CoreInGuiProcess)) { return CoreInGuiProcess; }
            if (m == coreModeToString(CoreExternal)) { return CoreExternal; }

            // some alternative names
            if (cm.contains("external")) { return CoreExternal; }
            if (cm.contains("gui")) { return CoreInGuiProcess; }
            return CoreInGuiProcess;
        }

        //! Core mode as string
        static QString coreModeToString(CoreMode mode)
        {
            switch (mode)
            {
            case CoreInGuiProcess: return QStringLiteral("coreinguiprocess");
            case CoreExternal:     return QStringLiteral("coreexternal");
            }
            return {};
        }
    };
}
#endif // guard
