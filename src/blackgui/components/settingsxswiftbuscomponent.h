// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSXSWIFTBUSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSXSWIFTBUSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "misc/simulation/settings/xswiftbussettings.h"
#include "misc/settingscache.h"
#include "misc/logcategories.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsXSwiftBusComponent;
}
namespace BlackGui::Components
{
    /*!
     * xswiftbus setup
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
        swift::misc::CSetting<swift::misc::simulation::settings::TXSwiftBusSettings> m_xSwiftBusSettings { this };

        void resetServer();
        void saveServer();
    };
} // ns
#endif // guard
