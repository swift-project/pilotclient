/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MAPPINGCOMPONENT_H
#define BLACKGUI_MAPPINGCOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/views/checkboxdelegate.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include <QTabWidget>
#include <QScopedPointer>
#include <QCompleter>

namespace Ui { class CMappingComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Mappings, models etc.
        class CMappingComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CMappingComponent();

            //! Number of current mappings
            int countCurrentMappings() const;

            //! Numer of models
            int countAircraftModels() const;

            //! \copydoc CListModelBase::getModelsStartsWith
            BlackMisc::Simulation::CAircraftModelList findModelsStartingWith(const QString modelName, Qt::CaseSensitivity cs);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private slots:
            //! Aircraft models available
            void ps_onAircraftModelsLoaded();

            //! Model matched
            void ps_onModelMatchingCompleted(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Changed count
            void ps_onRowCountChanged(int count, bool withFilter);

            //! Simulated aircraft did change in model
            //! \sa CViewBaseNonTemplate::objectChanged
            void ps_onChangedSimulatedAircraftInView(const BlackMisc::CVariant &simulatedAircraft, const BlackMisc::CPropertyIndex &index);

            //! Aircraft selected (in view)
            void ps_onAircraftSelectedInView(const QModelIndex &index);

            //! Model selected (in view)
            void ps_onModelSelectedInView(const QModelIndex &index);

            //! Save changed aircraft
            void ps_onSaveAircraft();

            //! Model preview
            void ps_onModelPreviewChanged(int state);

            //! Request update for mappings from backend
            void ps_onMappingsUpdateRequested();

            //! Request update for models from backend
            void ps_onModelsUpdateRequested();

            //! Rendered aircraft changed in backend
            void ps_onRenderedAircraftModelChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator);

            //! Aircraft enabled, disabled in backend
            void ps_onChangedAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator);

            //! Fast position updates on/off in backend
            void ps_onFastPositionUpdatesEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator);

            //! Connection status has been changed
            void ps_onConnectionStatusChanged(uint from, uint to);

            //! Fast position updates onf/off
            void ps_onMenuChangeFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Enable / disable aircraft
            void ps_onMenuEnableAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        private:
            static const QString &mappingtOriginator();
            QScopedPointer<Ui::CMappingComponent> ui;
            QCompleter *m_modelCompleter = nullptr;
            BlackGui::Views::CCheckBoxDelegate *m_currentMappingsViewDelegate = nullptr;
        };

    } // namespace
} // namespace

#endif // guard
