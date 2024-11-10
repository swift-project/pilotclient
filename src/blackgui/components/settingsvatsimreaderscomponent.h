// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H

#include "core/vatsim/vatsimsettings.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CSettingsVatsimReadersComponent;
}

namespace BlackGui::Components
{
    /*!
     * Settings for readers
     */
    class CSettingsVatsimReadersComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsVatsimReadersComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsVatsimReadersComponent();

    private:
        void onSettingsChanged();
        void save();
        void reload();
        void initValues();

        QScopedPointer<Ui::CSettingsVatsimReadersComponent> ui;
        swift::misc::CSetting<swift::core::vatsim::TVatsimDataFile> m_settingsDataFile { this, &CSettingsVatsimReadersComponent::onSettingsChanged };
        swift::misc::CSetting<swift::core::vatsim::TVatsimMetars> m_settingsMetars { this, &CSettingsVatsimReadersComponent::onSettingsChanged };
    };
} // ns

#endif // guard
