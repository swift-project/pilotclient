/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H

#include "blackcore/vatsim/vatsimsettings.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CSettingsVatsimReadersComponent; }

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
        BlackMisc::CSetting<BlackCore::Vatsim::TVatsimBookings> m_settingsBookings { this, &CSettingsVatsimReadersComponent::onSettingsChanged };
        BlackMisc::CSetting<BlackCore::Vatsim::TVatsimDataFile> m_settingsDataFile { this, &CSettingsVatsimReadersComponent::onSettingsChanged };
        BlackMisc::CSetting<BlackCore::Vatsim::TVatsimMetars>   m_settingsMetars   { this, &CSettingsVatsimReadersComponent::onSettingsChanged };
    };
} // ns

#endif // guard
