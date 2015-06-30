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

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "enableforruntime.h"
#include "enableforruntime.h"
#include <QFrame>

namespace Ui { class CSettingsSimulatorComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! All simulator settings component (GUI)
        class BLACKGUI_EXPORT CSettingsSimulatorComponent : public QFrame, public CEnableForRuntime
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
            void ps_pluginHasBeenSelectedInComboBox(int index);

            //! Apply max.aircraft
            void ps_onApplyMaxRenderedAircraft();

            //! Apply max.distance
            void ps_onApplyMaxRenderedDistance();

            //! Apply disable rendering
            void ps_onApplyDisableRendering();

            //! Apply time sync
            void ps_onApplyTimeSync();

            //! Clear restricted rendering
            void ps_clearRestricedRendering();

            //! Simulator plugin changed
            void ps_simulatorPluginChanged(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        private:

            //! Smarter way to set current driver, avoids unnecessary signals and less formatting dependend
            void setCurrentPluginInComboBox(const BlackMisc::Simulation::CSimulatorPluginInfo &plugin);

            //! Set the GUI values
            void setGuiValues();

            //! Available plugins, auto pseudo plugin added
            BlackMisc::Simulation::CSimulatorPluginInfoList getAvailablePlugins(bool plusAuto) const;

            QScopedPointer<Ui::CSettingsSimulatorComponent> ui; //!< UI
            bool m_pluginLoaded = false; //!< plugin loaded
        };
    }
} // namespace
#endif // guard
