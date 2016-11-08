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
#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
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
        class CUpdateTimer;

        //! Mappings, models etc.
        class BLACKGUI_EXPORT CMappingComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CMappingComponent();

            //! Number of current mappings
            int countCurrentMappings() const;

            //! Numer of models
            int countAircraftModels() const;

            //! Find models starting with
            BlackMisc::Simulation::CAircraftModelList findModelsStartingWith(const QString modelName, Qt::CaseSensitivity cs);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        private slots:
            //! Aircraft models available
            void ps_onModelSetChanged();

            //! Changed count
            void ps_onRowCountChanged(int count, bool withFilter);

            //! Simulated aircraft did change in view
            //! \sa CViewBaseNonTemplate::objectChanged
            void ps_onChangedSimulatedAircraftInView(const BlackMisc::CVariant &simulatedAircraft, const BlackMisc::CPropertyIndex &index);

            //! Aircraft selected (in view)
            void ps_onAircraftSelectedInView(const QModelIndex &index);

            //! Model selected (in view)
            void ps_onModelSelectedInView(const QModelIndex &index);

            //! Save changed aircraft
            void ps_onSaveAircraft();

            //! Reset aircraft
            void ps_onResetAircraft();

            //! Model preview
            void ps_onModelPreviewChanged(int state);

            //! Request update for models from backend
            void ps_onModelsUpdateRequested();

            //! Rendered aircraft changed in backend
            void ps_onRemoteAircraftModelChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Connection status has been changed
            void ps_onConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Fast position updates onf/off
            void ps_onMenuChangeFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Enable / disable aircraft
            void ps_onMenuToggleEnableAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Highlight in simulator
            void ps_onMenuHighlightInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Update with next cycle
            void ps_markRenderedAircraftForUpdate(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Update with next cycle
            void ps_markRenderedViewForUpdate();

            //! Adding a remote aircraft failed
            void ps_addingRemoteAircraftFailed(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CStatusMessage &message);

        private:
            //! Identifier for data send from this component
            BlackMisc::CIdentifier mappingIdentifier();

            //! Update simulated aircraft view
            void updateRenderedAircraftView(bool forceUpdate = false);

            //! Check callsign entered
            BlackMisc::Aviation::CCallsign validateRenderedCallsign() const;

            QScopedPointer<Ui::CMappingComponent> ui;
            bool                                  m_missedRenderedAircraftUpdate = true;
            QScopedPointer<CUpdateTimer>          m_updateTimer;
            BlackGui::Views::CCheckBoxDelegate   *m_currentMappingsViewDelegate = nullptr;
            BlackMisc::CIdentifier                m_identifier;

        private slots:
            //! Updated by timer
            void ps_backgroundUpdate();
        };

    } // namespace
} // namespace

#endif // guard
