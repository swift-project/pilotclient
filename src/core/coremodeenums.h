// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_GUIMODEENUMS_H
#define SWIFT_GUI_GUIMODEENUMS_H

#include <QString>

namespace swift::core
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
} // namespace swift::core
#endif // SWIFT_GUI_GUIMODEENUMS_H
