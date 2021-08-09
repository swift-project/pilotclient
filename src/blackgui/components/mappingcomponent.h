/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_MAPPINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_MAPPINGCOMPONENT_H

#include "blackgui/settings/viewupdatesettings.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/tokenbucket.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QTimer>
#include <QString>
#include <Qt>

class QModelIndex;

namespace Ui { class CMappingComponent; }
namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}
namespace BlackGui
{
    namespace Views { class CCheckBoxDelegate; }
    namespace Components
    {
        //! Mappings, models etc.
        class BLACKGUI_EXPORT CMappingComponent :
            public COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackMisc::CIdentifiable
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
            BlackMisc::Simulation::CAircraftModelList findModelsStartingWith(const QString &modelName, Qt::CaseSensitivity cs);

        signals:
            //! Request the validation dialog
            void requestValidationDialog();

        private:
            //! Aircraft models available
            void onModelSetChanged(const BlackMisc::Simulation::CSimulatorInfo &dummy);

            //! Changed count of rendered or aircraft model count
            void onRowCountChanged(int count, bool withFilter);

            //! Simulated aircraft did change in view
            //! \sa CViewBaseNonTemplate::objectChanged
            void onChangedSimulatedAircraftInView(const BlackMisc::CVariant &simulatedAircraft, const BlackMisc::CPropertyIndex &index);

            //! Aircraft selected (in view)
            void onAircraftSelectedInView(const QModelIndex &index);

            //! Model selected (in view)
            void onModelSelectedInView(const QModelIndex &index);

            //! Save changed aircraft
            void onSaveAircraft();

            //! Reset aircraft
            void onResetAircraft();

            //! Model preview
            void onModelPreviewChanged(int state);

            //! Request update for models from backend
            void onModelsUpdateRequested();

            //! Request temp.disablng of models (for matching)
            void onTempDisableModelsForMatchingRequested(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Rendered aircraft changed in backend
            void onRemoteAircraftModelChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Connection status has been changed
            void onConnectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

            //! Widget changed
            void onTabWidgetChanged(int index);

            //! Selected or connected simulator
            BlackMisc::Simulation::CSimulatorInfo getConnectedOrSelectedSimulator() const;

            //! Is simulator avialable?
            bool isSimulatorAvailable() const;

            //! Show / hide model details
            void showAircraftModelDetails(bool show);

            //! Adding a remote aircraft failed
            void onAddingRemoteAircraftFailed(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool disabled, bool failover, const BlackMisc::CStatusMessage &message);

            //! Timer update
            void timerUpdate();

            //! Token bucket based update
            void tokenBucketUpdate();

            //! Token bucket based update
            void tokenBucketUpdateAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Settings have been changed
            void settingsChanged();

            //! Connection status has been changed
            void onNetworkConnectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

            //! Update simulated aircraft view
            void updateRenderedAircraftView(bool forceUpdate = false);

            //! Check callsign entered
            BlackMisc::Aviation::CCallsign validateRenderedCallsign();

            //! Changed selector
            void onModelSetSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Plugin info has been changed
            void onSimulatorPluginChanged(const BlackMisc::Simulation::CSimulatorPluginInfo &pluginInfo);

            //! Simulator connected
            void onSimulatorStatusChanged(int status);

            //! Do re-matching
            void doMatchingsAgain();

            static constexpr int OverlayMessageMs = 5000;
            QScopedPointer<Ui::CMappingComponent> ui;
            QTimer m_updateTimer;
            bool m_missedRenderedAircraftUpdate = true; //! Rendered aircraft need update
            BlackMisc::CTokenBucket m_bucket { 3, 5000, 1};
            BlackMisc::CSettingReadOnly<Settings::TViewUpdateSettings> m_settings { this, &CMappingComponent::settingsChanged }; //!< settings changed
            Views::CCheckBoxDelegate *m_currentMappingsViewDelegate = nullptr; //! checkbox in view
        };
    } // namespace
} // namespace

#endif // guard
