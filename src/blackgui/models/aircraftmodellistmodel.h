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

#include <QBrush>
#include <QStringList>
#include <QVariant>
#include <Qt>

class QModelIndex;

namespace BlackMisc { namespace Simulation { class CAircraftModel; } }
namespace BlackGui
{
    namespace Models
    {
        //! Aircraft model list model
        class BLACKGUI_EXPORT CAircraftModelListModel :
            public COrderableListModelDbObjects<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int, true>
        {
        public:
            //! How to display
            //! \sa BlackMisc::Simulation::CAircraftModel::ModelType
            enum AircraftModelMode
            {
                NotSet,
                OwnAircraftModelClient,      //!< models existing for my simulator
                OwnAircraftModelMappingTool, //!< models of my simulator, but in mapping mode
                OwnModelSet,                 //!< own model set
                StashModel,                  //!< stashed models
                Database,                    //!< Database entry
                VPilotRuleModel              //!< vPilot rule turned into model
            };

            //! Constructor
            explicit CAircraftModelListModel(AircraftModelMode mode, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAircraftModelListModel() override {}

            //! Mode
            void setAircraftModelMode(CAircraftModelListModel::AircraftModelMode stationMode);

            //! Mode
            AircraftModelMode getModelMode() const { return m_mode; }

            //! Highlight models
            void setHighlightModelStrings(const QStringList &modelStrings = QStringList());

            //! Highlight models
            bool highlightModelStrings() const { return m_highlightModelStrings; }

            //! Highlight models
            void setHighlightModelStrings(bool highlightModelStrings);

            //! The highlight color
            void setHighlightModelStringsColor(const QBrush &brush) { m_highlightColor = brush; }

            //! \copydoc CListModelBaseNonTemplate::clearHighlighting
            virtual void clearHighlighting() override;

            //! \copydoc CListModelBaseNonTemplate::hasHighlightedRows
            virtual bool hasHighlightedRows() const override;

            //! Model strings
            QStringList getModelStrings(bool sort) const;

            //! Replace models with same model string, or just add
            void replaceOrAddByModelString(const BlackMisc::Simulation::CAircraftModelList &models);

            //! \copydoc QAbstractItemModel::data
            virtual QVariant data(const QModelIndex &index, int role) const override;

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::isOrderable
            virtual bool isOrderable() const override { return true; }

        private:
            AircraftModelMode m_mode = NotSet;                  //!< current mode
            bool              m_highlightModelStrings = false;  //!< highlight if in m_highlightStrings
            QStringList       m_highlightStrings;               //!< model strings to highlight
            QBrush            m_highlightColor{Qt::yellow};     //!< how to highlight
        };
    } // ns
} // ns
#endif // guard
