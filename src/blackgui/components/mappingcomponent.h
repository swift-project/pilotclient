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

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private slots:
            //! Aircraft models available
            void ps_onAircraftModelsLoaded();

            //! Mappings changed
            void ps_onMappingsChanged();

            //! Model matched
            void ps_modelMatched(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Changed count
            void ps_onRowCountChanged(int count, bool withFilter);

            //! Simulated aircraft did change in model
            //! \sa CViewBaseNonTemplate::objectChanged
            void ps_onChangedSimulatedAircraft(const BlackMisc::CVariant &simulatedAircraft, const BlackMisc::CPropertyIndex &index);

            //! Aircraft selected (in view)
            void ps_onAircraftSelectedInView(const QModelIndex &index);

            //! Model selected (in view)
            void ps_onModelSelectedInView(const QModelIndex &index);

            //! Apply new max remote aircraft
            void ps_onApplyNewMaxRemoteAircraft();

            //! Save changed aircraft
            void ps_onSaveAircraft();

            //! Model preview
            void ps_onModelPreviewChanged(int state);

            //! Requested update for mappings from backend
            void ps_onMappingsUpdateRequested();

            //! Request update for models from backend
            void ps_onModelsUpdateRequested();

            //! Connection status has been changed
            void ps_onConnectionStatusChanged(uint from, uint to);

        private:
            QScopedPointer<Ui::CMappingComponent> ui;
            QCompleter *m_modelCompleter = nullptr;
            BlackGui::Views::CCheckBoxDelegate *m_currentMappingsViewDelegate = nullptr;

        };

    } // namespace
} // namespace

#endif // guard
