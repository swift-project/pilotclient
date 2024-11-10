// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SETTINGS_DATASETTINGSCOMPONENT_H
#define BLACKGUI_SETTINGS_DATASETTINGSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessagesframe.h"
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui
{
    class CDataSettingsComponent;
}
namespace swift::core::db
{
    class CBackgroundDataUpdater;
}
namespace BlackGui::Components
{
    /*!
     * Settings
     */
    class BLACKGUI_EXPORT CDataSettingsComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDataSettingsComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDataSettingsComponent();

        //! Background updater
        void setBackgroundUpdater(const swift::core::db::CBackgroundDataUpdater *updater);

    private:
        QScopedPointer<Ui::CDataSettingsComponent> ui;
    };
} // ns

#endif // guard
