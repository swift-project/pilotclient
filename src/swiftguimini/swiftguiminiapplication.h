// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINIAPPLICATION_H
#define SWIFTGUIMINIAPPLICATION_H

#include <QObject>

#include "gui/guiapplication.h"

/*!
 * Stark vereinfachte GUI Anwendung.
 * Laeuft immer im lokalen Modus (kein DBus, kein Remote-Core, keine Core-Mode Auswahl).
 */
class CSwiftGuiMiniApplication : public swift::gui::CGuiApplication
{
    Q_OBJECT

public:
    //! Constructor
    CSwiftGuiMiniApplication();

    //! Single instance
    static CSwiftGuiMiniApplication *instance();

protected:
    //! \copydoc swift::core::CApplication::startHookIn
    swift::misc::CStatusMessageList startHookIn() override;
};

#endif // SWIFTGUIMINIAPPLICATION_H