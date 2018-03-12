/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSVATSIMREADERSCOMPONENT_H

#include "blackcore/vatsim/vatsimsettings.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CSettingsVatsimReadersComponent; }

namespace BlackGui
{
    namespace Components
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

        private slots:
            void ps_settingsChanged();
            void ps_save();
            void ps_reload();

        private:
            void initValues();

            QScopedPointer<Ui::CSettingsVatsimReadersComponent> ui;
            BlackMisc::CSetting<BlackCore::Vatsim::TVatsimBookings> m_settingsBookings { this, &CSettingsVatsimReadersComponent::ps_settingsChanged };
            BlackMisc::CSetting<BlackCore::Vatsim::TVatsimDataFile> m_settingsDataFile { this, &CSettingsVatsimReadersComponent::ps_settingsChanged };
            BlackMisc::CSetting<BlackCore::Vatsim::TVatsimMetars>   m_settingsMetars   { this, &CSettingsVatsimReadersComponent::ps_settingsChanged };
        };
    } // ns
} // ns

#endif // guard
