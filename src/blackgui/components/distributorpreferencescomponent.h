/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DISTRIBUTORPREFERENCESCOMPONENT_H
#define BLACKGUI_COMPONENTS_DISTRIBUTORPREFERENCESCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/settingscache.h"

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
            void onSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            // Init
            void triggerDeferredSimulatorChange();

        private:
            QScopedPointer<Ui::CDistributorPreferencesComponent> ui;
            BlackMisc::CSetting<BlackMisc::Simulation::Settings::TDistributorListPreferences> m_distributorPreferences { this, &CDistributorPreferencesComponent::onPreferencesChanged };

            //! Update
            void updateContainerMaybeAsync(const BlackMisc::Simulation::CDistributorList &models, bool sortByOrder = true);
        };
    } // ns
} // ns

#endif // guard
