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
#include "blackgui/models/listmodeldbobjects.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include <QStringList>
#include <QAbstractItemModel>

namespace BlackGui
{
    namespace Models
    {
        //! Aircraft model list model
        class BLACKGUI_EXPORT CAircraftModelListModel :
            public CListModelDbObjects<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int, true>
        {
        public:
            //! How to display
            //! \sa BlackMisc::Simulation::CAircraftModel::ModelType
            enum AircraftModelMode
            {
                NotSet,
                OwnSimulatorModel,        ///< models existing for my simulator
                OwnSimulatorModelMapping, ///< models of my simulator, but in mapping mode
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

            //! Highlight models
            bool highlightGivenModelStrings() const { return m_highlightModelStrings; }

            //! Highlight models
            void setHighlightModelsStrings(const QStringList &modelStrings = QStringList()) { m_highlightStrings = modelStrings; }

            //! Highlight models
            void setHighlightModelsStrings(bool highlightModelStrings) { m_highlightModelStrings = highlightModelStrings; }

            //! Model strings
            QStringList getModelStrings(bool sort) const;

            //! Replace models with same model string, or just add
            void replaceOrAddByModelString(const BlackMisc::Simulation::CAircraftModelList &models);

            //! \copydoc QAbstractItemModel::data
            virtual QVariant data(const QModelIndex &index, int role) const override;

        private:
            AircraftModelMode m_mode = NotSet;    //!< current mode
            bool m_highlightModelStrings = false; //!< highlight in in model strings
            QStringList m_highlightStrings;       //!< model strings to highlight
        };
    } // ns
} // ns
#endif // guard
