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
            Standalone,
            Distributed
        };

        //! String to core mode
        static CoreMode stringToCoreMode(const QString &m)
        {
            QString cm(m.toLower().trimmed());
            if (cm.isEmpty()) { return Standalone; }
            if (m == coreModeToString(Standalone)) { return Standalone; }
            if (m == coreModeToString(Distributed)) { return Distributed; }

            // some alternative names
            if (cm.contains("distribute")) { return Distributed; }
            if (cm.contains("standalone")) { return Standalone; }
            if (cm.contains("external")) { return Distributed; }
            if (cm.contains("gui")) { return Standalone; }
            return Standalone;
        }

        //! Core mode as string
        static QString coreModeToString(CoreMode mode)
        {
            switch (mode)
            {
            case Standalone: return QStringLiteral("standalone");
            case Distributed: return QStringLiteral("distributed");
            }
            return {};
        }
    };
}
#endif // guard
