// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_DATASETTINGSCOMPONENT_H
#define SWIFT_GUI_SETTINGS_DATASETTINGSCOMPONENT_H

#include <QObject>
#include <QScopedPointer>

#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"

class QWidget;

namespace Ui
{
    class CDataSettingsComponent;
}
namespace swift::core::db
{
    class CBackgroundDataUpdater;
}
namespace swift::gui::components
{
    /*!
     * Settings
     */
    class SWIFT_GUI_EXPORT CDataSettingsComponent : public COverlayMessagesFrame
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
} // namespace swift::gui::components

#endif // SWIFT_GUI_SETTINGS_DATASETTINGSCOMPONENT_H
