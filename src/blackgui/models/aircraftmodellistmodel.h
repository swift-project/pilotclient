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
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>

namespace BlackGui
{
    namespace Models
    {
        //! Aircraft model list model
        class BLACKGUI_EXPORT CAircraftModelListModel : public CListModelBase<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>
        {
        public:
            //! How to display
            //! \sa BlackMisc::Simulation::CAircraftModel::ModelType
            enum AircraftModelMode
            {
                NotSet,
                OwnSimulatorModel, ///< model existing with my sim
                MappedModel,       ///< Model based on mapping operation
                Database,          ///< Database entry
                VPilotRuleModel    ///< vPilot rule turned into model
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

        protected:
            //! \copydoc QAbstractItemModel::data
            virtual QVariant data(const QModelIndex &index, int role) const override;

        private:
            AircraftModelMode m_mode = NotSet; //!< current mode
            bool m_highlightDbData = false;
        };
    } // ns
} // ns
#endif // guard
