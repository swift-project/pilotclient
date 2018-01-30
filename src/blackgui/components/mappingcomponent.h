/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_MAPPINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_MAPPINGCOMPONENT_H

#include "blackcore/network.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/settings/viewupdatesettings.h"
#include "blackmisc/tokenbucket.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QTimer>
#include <QString>
#include <Qt>

class QCompleter;
class QModelIndex;
class QWidget;

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}
namespace Ui { class CMappingComponent; }
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
            //! Constructor
            explicit CMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CMappingComponent();

            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Number of current mappings
            int countCurrentMappings() const;

            //! Numer of models
            int countAircraftModels() const;

            //! Find models starting with
            BlackMisc::Simulation::CAircraftModelList findModelsStartingWith(const QString modelName, Qt::CaseSensitivity cs);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        private:
            //! Aircraft models available
            void onModelSetChanged();

            //! Changed count
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

            //! Rendered aircraft changed in backend
            void onRemoteAircraftModelChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Connection status has been changed
            void onConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Fast position updates onf/off
            void onMenuChangeFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Enable / disable aircraft
            void onMenuToggleEnableAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Highlight in simulator
            void onMenuHighlightInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Adding a remote aircraft failed
            void addingRemoteAircraftFailed(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CStatusMessage &message);

            //! Timer update
            void timerUpdate();

            //! Token bucket based update
            void tokenBucketUpdate();

            //! Token bucket based update
            void tokenBucketUpdateAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Settings have been changed
            void settingsChanged();

            //! Connection status has been changed
            void connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Identifier for data send from this component
            const BlackMisc::CIdentifier &mappingIdentifier() const { return m_identifier; }

            //! Update simulated aircraft view
            void updateRenderedAircraftView(bool forceUpdate = false);

            //! Check callsign entered
            BlackMisc::Aviation::CCallsign validateRenderedCallsign();

            static constexpr int OverlayMessageMs = 5000;
            QScopedPointer<Ui::CMappingComponent> ui;
            bool m_missedRenderedAircraftUpdate = true; //! Rendered aircraft need update
            QTimer m_updateTimer;
            BlackMisc::CTokenBucket m_bucket { 3, BlackMisc::PhysicalQuantities::CTime(5.0, BlackMisc::PhysicalQuantities::CTimeUnit::s()), 1};
            Views::CCheckBoxDelegate *m_currentMappingsViewDelegate = nullptr; //! checkbox in view
            BlackMisc::CIdentifier m_identifier { "MappingComponent", this };
            BlackMisc::CSettingReadOnly<BlackGui::Settings::TViewUpdateSettings> m_settings { this, &CMappingComponent::settingsChanged }; //!< settings changed
        };
    } // namespace
} // namespace

#endif // guard
