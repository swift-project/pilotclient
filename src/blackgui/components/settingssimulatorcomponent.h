/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_SETTINGSSIMULATORCOMPONENT_H
#define BLACKGUI_SETTINGSSIMULATORCOMPONENT_H

#include "blacksim/setsimulator.h"
#include "enableforruntime.h"
#include "enableforruntime.h"
#include <QFrame>

namespace Ui { class CSettingsSimulatorComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * All simulator settings component (GUI)
         */
        class CSettingsSimulatorComponent : public QFrame, public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSettingsSimulatorComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsSimulatorComponent();

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet()
            virtual void runtimeHasBeenSet() override;

        private slots:
            //! Driver changed
            void ps_pluginHasChanged(int index);

            //! Settings have been changed
            void ps_settingsHaveChanged(uint settingsType);

            //! Apply max.aircraft
            void ps_onApplyMaxRenderedAircraft();

            //! Apply max.distance
            void ps_onApplyMaxRenderedDistance();

            //! Apply time sync
            void ps_onApplyTimeSync();

            //! Restricted number of rendered aircraft
            void ps_onRenderingRestricted(bool restricted);

            //! Clear restricted rendering
            void ps_clearRestricedRendering();

        private:
            QScopedPointer<Ui::CSettingsSimulatorComponent> ui; //!< UI

            //! Smarter way to set current driver, avoids unnecessary signals and less formatting dependend
            void setCurrentPlugin(const BlackSim::CSimulatorPluginInfo &plugin);

            //! Set the GUI values
            void setRestrictedValues();

        };
    }
} // namespace
#endif // guard
