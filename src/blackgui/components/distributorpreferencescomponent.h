// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DISTRIBUTORPREFERENCESCOMPONENT_H
#define BLACKGUI_COMPONENTS_DISTRIBUTORPREFERENCESCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "misc/simulation/settings/modelsettings.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/settingscache.h"

#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDistributorPreferencesComponent;
}
namespace BlackGui
{
    class COverlayMessagesFrame;

    namespace Components
    {
        /*!
         * Set and order distributors (to be used for model set)
         */
        class CDistributorPreferencesComponent : public COverlayMessagesFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDistributorPreferencesComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDistributorPreferencesComponent();

        private:
            //! Changed preferences
            void onPreferencesChanged();

            //! Load all distributors
            void loadAllDistributors();

            //! Load all distributors for current simulator
            void loadDistributorsForSimulator();

            //! Load distributors from set
            void loadDistributorsFromSet();

            //! Save the preferences
            void save();

            //! Simulator has been changed
            void onSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

            // Init
            void triggerDeferredSimulatorChange();

        private:
            QScopedPointer<Ui::CDistributorPreferencesComponent> ui;
            swift::misc::CSetting<swift::misc::simulation::settings::TDistributorListPreferences> m_distributorPreferences { this, &CDistributorPreferencesComponent::onPreferencesChanged };

            //! Update
            void updateContainerMaybeAsync(const swift::misc::simulation::CDistributorList &models, bool sortByOrder = true);
        };
    } // ns
} // ns

#endif // guard
