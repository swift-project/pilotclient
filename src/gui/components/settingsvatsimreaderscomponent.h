// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "core/vatsim/vatsimsettings.h"

namespace Ui
{
    class CSettingsVatsimReadersComponent;
}

namespace swift::gui::components
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
        swift::misc::CSetting<swift::core::vatsim::TVatsimDataFile> m_settingsDataFile {
            this, &CSettingsVatsimReadersComponent::onSettingsChanged
        };
        swift::misc::CSetting<swift::core::vatsim::TVatsimMetars> m_settingsMetars {
            this, &CSettingsVatsimReadersComponent::onSettingsChanged
        };
    };
} // namespace swift::gui::components

#endif // guard
