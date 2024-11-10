// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENT_MODELMATCHERCOMPONENT_H
#define BLACKGUI_COMPONENT_MODELMATCHERCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/aircraftmatcher.h"
#include "misc/simulation/settings/modelmatchersettings.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/network/entityflags.h"
#include "misc/settingscache.h"

#include <QFrame>
#include <QObject>
#include <QPointer>
#include <QScopedPointer>

namespace Ui
{
    class CModelMatcherComponent;
}
namespace BlackGui
{
    namespace Views
    {
        class CAircraftModelView;
    }
    namespace Components
    {
        class CSettingsMatchingDialog;

        /*!
         * Model matcher testing and configuration
         */
        class BLACKGUI_EXPORT CModelMatcherComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelMatcherComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CModelMatcherComponent() override;

            //! Tab (where this component is embedded) has been changed
            void tabIndexChanged(int index);

            //! Set an external
            void setWorkbenchView(Views::CAircraftModelView *workbenchView);

        private:
            //! Simulator switched
            void onSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Workbench toggled
            void onWorkbenchToggled(bool checked);

            //! Cache changed
            void onCacheChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Web data have been read
            void onWebDataRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

            //! Display settings dialog
            void displaySettingsDialog();

            //! Run the matcher
            void testModelMatching();

            //! Reverse lookup
            //! \sa CAirspaceMonitor::reverseLookupModelWithFlightplanData is the real world lookup
            void reverseLookup();

            //! Init
            void redisplay();

            //! The current model set models
            swift::misc::simulation::CAircraftModelList getModelSetModels() const;

            //! The current model set models size
            int getMatcherModelsCount() const;

            //! Use workbench data
            bool useWorkbench() const;

            //! Pseudo aircraft created from entries
            swift::misc::simulation::CSimulatedAircraft createAircraft() const;

            //! Pseudo default aircraft
            swift::misc::simulation::CAircraftModel defaultModel() const;

            //! Run matching script
            static swift::misc::simulation::MatchingScriptReturnValues matchingScript(const swift::misc::simulation::CAircraftModel &inModel, const swift::misc::simulation::CAircraftMatcherSetup &setup, const swift::misc::simulation::CAircraftModelList &modelSet, swift::misc::CStatusMessageList &msgs);

            QScopedPointer<Ui::CModelMatcherComponent> ui;
            QPointer<Views::CAircraftModelView> m_workbenchView; //!< an external workbenc
            CSettingsMatchingDialog *m_settingsDialog = nullptr; //!< matching settings as dialog
            BlackCore::CAircraftMatcher m_matcher { this }; //!< used matcher
            swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TModelMatching> m_matchingSettings { this }; //!< settings
        };
    } // ns
} // ns

#endif // guard
