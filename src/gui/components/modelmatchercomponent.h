// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENT_MODELMATCHERCOMPONENT_H
#define SWIFT_GUI_COMPONENT_MODELMATCHERCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QPointer>
#include <QScopedPointer>

#include "core/aircraftmatcher.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"
#include "misc/settingscache.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/settings/modelmatchersettings.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatorinfo.h"

namespace Ui
{
    class CModelMatcherComponent;
}
namespace swift::gui
{
    namespace views
    {
        class CAircraftModelView;
    }
    namespace components
    {
        class CSettingsMatchingDialog;

        /*!
         * Model matcher testing and configuration
         */
        class SWIFT_GUI_EXPORT CModelMatcherComponent : public QFrame
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
            void setWorkbenchView(views::CAircraftModelView *workbenchView);

        private:
            //! Simulator switched
            void onSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Workbench toggled
            void onWorkbenchToggled(bool checked);

            //! Cache changed
            void onCacheChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Web data have been read
            void onWebDataRead(swift::misc::network::CEntityFlags::Entity entity,
                               swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

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
            static swift::misc::simulation::MatchingScriptReturnValues
            matchingScript(const swift::misc::simulation::CAircraftModel &inModel,
                           const swift::misc::simulation::CAircraftMatcherSetup &setup,
                           const swift::misc::simulation::CAircraftModelList &modelSet,
                           swift::misc::CStatusMessageList &msgs);

            QScopedPointer<Ui::CModelMatcherComponent> ui;
            QPointer<views::CAircraftModelView> m_workbenchView; //!< an external workbenc
            CSettingsMatchingDialog *m_settingsDialog = nullptr; //!< matching settings as dialog
            swift::core::CAircraftMatcher m_matcher { this }; //!< used matcher
            swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TModelMatching> m_matchingSettings {
                this
            }; //!< settings
        };
    } // namespace components
} // namespace swift::gui

#endif // SWIFT_GUI_COMPONENT_MODELMATCHERCOMPONENT_H
