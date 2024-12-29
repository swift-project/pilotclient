// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MAPPINGCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_MAPPINGCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>
#include <Qt>

#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/overlaymessagesframe.h"
#include "gui/settings/viewupdatesettings.h"
#include "gui/swiftguiexport.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/network/connectionstatus.h"
#include "misc/propertyindex.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/tokenbucket.h"
#include "misc/variant.h"

class QModelIndex;

namespace Ui
{
    class CMappingComponent;
}
namespace swift::misc
{
    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
    {
        class CSimulatedAircraft;
    }
} // namespace swift::misc
namespace swift::gui
{
    namespace views
    {
        class CCheckBoxDelegate;
    }
    namespace components
    {
        //! Mappings, models etc.
        class SWIFT_GUI_EXPORT CMappingComponent :
            public COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public swift::misc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Tab widget
            //! \remark needs to be in sync with tab order
            enum TabWidget
            {
                TabRenderedAircraft = 0,
                TabAircraftModels,
                TabInterpolatorSetup,
                TabStatistics,
                TabMatchingLog,
                TabPartsLog
            };

            //! Log categories
            static const QStringList &getLogCategories();

            //! Constructor
            explicit CMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CMappingComponent() override;

            //! Number of current mappings
            int countCurrentMappings() const;

            //! Numer of models
            int countAircraftModels() const;

            //! Set tab
            void setTab(TabWidget tab);

            //! Find models starting with
            swift::misc::simulation::CAircraftModelList findModelsStartingWith(const QString &modelName,
                                                                               Qt::CaseSensitivity cs);

        signals:
            //! Request the validation dialog
            void requestValidationDialog();

        private:
            //! Aircraft models available
            void onModelSetChanged(const swift::misc::simulation::CSimulatorInfo &dummy);

            //! Changed count of rendered or aircraft model count
            void onRowCountChanged(int count, bool withFilter);

            //! Simulated aircraft did change in view
            //! \sa CViewBaseNonTemplate::objectChanged
            void onChangedSimulatedAircraftInView(const swift::misc::CVariant &simulatedAircraft,
                                                  const swift::misc::CPropertyIndex &index);

            //! Aircraft selected (in view)
            void onAircraftSelectedInView(const QModelIndex &index);

            //! Save changed aircraft
            void onSaveAircraft();

            //! Reset aircraft
            void onResetAircraft();

            //! Model preview
            void onModelPreviewChanged(int state);

            //! Request update for models from backend
            void onModelsUpdateRequested();

            //! Request temp.disablng of models (for matching)
            void onTempDisableModelsForMatchingRequested(const swift::misc::simulation::CAircraftModelList &models);

            //! Rendered aircraft changed in backend
            void onRemoteAircraftModelChanged(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                              const swift::misc::CIdentifier &originator);

            //! Connection status has been changed
            void onConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                           const swift::misc::network::CConnectionStatus &to);

            //! Widget changed
            void onTabWidgetChanged(int index);

            //! Selected or connected simulator
            swift::misc::simulation::CSimulatorInfo getConnectedOrSelectedSimulator() const;

            //! Is simulator avialable?
            bool isSimulatorAvailable() const;

            //! Show / hide model details
            void showAircraftModelDetails(bool show);

            //! Adding a remote aircraft failed
            void onAddingRemoteAircraftFailed(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                              bool disabled, bool failover, const swift::misc::CStatusMessage &message);

            //! Timer update
            void timerUpdate();

            //! Token bucket based update
            void tokenBucketUpdate();

            //! Token bucket based update
            void tokenBucketUpdateAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft);

            //! Settings have been changed
            void settingsChanged();

            //! Connection status has been changed
            void onNetworkConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                                  const swift::misc::network::CConnectionStatus &to);

            //! Update simulated aircraft view
            void updateRenderedAircraftView(bool forceUpdate = false);

            //! Check callsign entered
            swift::misc::aviation::CCallsign validateRenderedCallsign();

            //! Changed selector
            void onModelSetSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Plugin info has been changed
            void onSimulatorPluginChanged(const swift::misc::simulation::CSimulatorPluginInfo &pluginInfo);

            //! Simulator connected
            void onSimulatorStatusChanged(int status);

            //! Do re-matching
            void doMatchingsAgain();

            static constexpr std::chrono::milliseconds OverlayMessageMs { 5000 };
            QScopedPointer<Ui::CMappingComponent> ui;
            QTimer m_updateTimer;
            bool m_missedRenderedAircraftUpdate = true; //! Rendered aircraft need update
            swift::misc::CTokenBucket m_bucket { 3, 5000, 1 };
            swift::misc::CSettingReadOnly<settings::TViewUpdateSettings> m_settings {
                this, &CMappingComponent::settingsChanged
            }; //!< settings changed
            views::CCheckBoxDelegate *m_currentMappingsViewDelegate = nullptr; //! checkbox in view
        };
    } // namespace components
} // namespace swift::gui

#endif // SWIFT_GUI_COMPONENTS_MAPPINGCOMPONENT_H
