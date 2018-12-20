/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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
            CoreExternalCoreAudio,
            CoreExternalAudioGui
        };

        //! String to core mode
        static CoreMode stringToCoreMode(const QString &m)
        {
            QString cm(m.toLower().trimmed());
            if (cm.isEmpty()) { return CoreInGuiProcess; }
            if (m == coreModeToString(CoreExternalCoreAudio)) { return CoreExternalCoreAudio; }
            if (m == coreModeToString(CoreInGuiProcess)) { return CoreInGuiProcess; }
            if (m == coreModeToString(CoreExternalAudioGui)) { return CoreExternalAudioGui; }

            // some alternative names
            if (cm.contains("audiolocal")) { return CoreExternalAudioGui; }
            if (cm.contains("localaudio")) { return CoreExternalAudioGui; }
            if (cm.contains("external")) { return CoreExternalCoreAudio; }
            if (cm.contains("gui")) { return CoreInGuiProcess; }
            return CoreInGuiProcess;
        }

        //! Core mode as string
        static QString coreModeToString(CoreMode mode)
        {
            switch (mode)
            {
            case CoreInGuiProcess: return QStringLiteral("coreinguiprocess");
            case CoreExternalCoreAudio: return QStringLiteral("coreexternal");
            case CoreExternalAudioGui: return QStringLiteral("coreexternalaudiogui");
            }
            return {};
        }
    };
}
#endif // guard
