// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSXSWIFTBUSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSXSWIFTBUSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/settings/xswiftbussettings.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/logcategories.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsXSwiftBusComponent;
}
namespace BlackGui::Components
{
    /*!
     * XSwiftBus setup
     */
    class BLACKGUI_EXPORT CSettingsXSwiftBusComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CSettingsXSwiftBusComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSettingsXSwiftBusComponent() override;

    private:
        QScopedPointer<Ui::CSettingsXSwiftBusComponent> ui;
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TXSwiftBusSettings> m_xSwiftBusSettings { this };

        void resetServer();
        void saveServer();
    };
} // ns
#endif // guard
