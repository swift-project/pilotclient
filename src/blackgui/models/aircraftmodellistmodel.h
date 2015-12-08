/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTMODELLISTMODEL_H
#define BLACKGUI_AIRCRAFTMODELLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackgui/models/modelswithdbkey.h"
#include <QAbstractItemModel>

namespace BlackGui
{
    namespace Models
    {
        //! Aircraft model list model
        class BLACKGUI_EXPORT CAircraftModelListModel :
            public CModelsWithDbKeysBase<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int, true>
        {
        public:
            //! How to display
            //! \sa BlackMisc::Simulation::CAircraftModel::ModelType
            enum AircraftModelMode
            {
                NotSet,
                OwnSimulatorModel,        ///< models existing for my simulator
                OwnSimulatorModelMapping, ///< models of my simulator, but in mapping mode
                MappedModel,              ///< model based on mapping operation
                Database,                 ///< Database entry
                VPilotRuleModel,          ///< vPilot rule turned into model
                StashModel                ///< stashed models
            };

            //! Constructor
            explicit CAircraftModelListModel(AircraftModelMode mode, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftModelListModel() {}

            //! Mode
            void setAircraftModelMode(CAircraftModelListModel::AircraftModelMode stationMode);

            //! Mode
            AircraftModelMode getModelMode() const { return m_mode; }

            //! Highlight the DB models
            bool highlightDbData() const { return m_highlightDbData; }

            //! Highlight the DB models
            void setHighlightDbData(bool highlightDbData) { m_highlightDbData = highlightDbData; }

            //! Highlight stashed models
            bool highlightStashedModels() const { return m_highlightStashedData; }

            //! Highlight stashed models
            void setHighlightStashedModels(bool highlightStashedModels) { m_highlightStashedData = highlightStashedModels; }

        protected:
            //! \copydoc QAbstractItemModel::data
            virtual QVariant data(const QModelIndex &index, int role) const override;

        private:
            AircraftModelMode m_mode = NotSet; //!< current mode
            bool m_highlightDbData = false;
            bool m_highlightStashedData = false;
        };
    } // ns
} // ns
#endif // guard
