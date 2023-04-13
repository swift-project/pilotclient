/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTMODELLISTMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTMODELLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeldbobjects.h"
#include "blackmisc/simulation/aircraftmodellist.h"

#include <QBrush>
#include <QStringList>
#include <QVariant>
#include <Qt>

class QModelIndex;

namespace BlackMisc::Simulation
{
    class CAircraftModel;
}
namespace BlackGui::Models
{
    //! Aircraft model list model
    class BLACKGUI_EXPORT CAircraftModelListModel :
        public COrderableListModelDbObjects<BlackMisc::Simulation::CAircraftModelList, int, true>
    {
        Q_OBJECT

    public:
        //! How to display
        //! \sa BlackMisc::Simulation::CAircraftModel::ModelType
        enum AircraftModelMode
        {
            NotSet,
            OwnAircraftModelClient, //!< models existing for my simulator
            OwnAircraftModelMappingTool, //!< models of my simulator, but in mapping mode
            OwnModelSet, //!< own model set
            StashModel, //!< stashed models
            Database, //!< Database entry
            VPilotRuleModel //!< vPilot rule turned into model
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
        void setHighlightModels(const BlackMisc::Simulation::CAircraftModelList &highlightModels);

        //! Highlight models
        bool highlightModels() const { return m_highlightModels; }

        //! Highlight models
        void setHighlight(bool highlightModels);

        //! The highlight color
        void setHighlightColor(const QBrush &brush) { m_highlightColor = brush; }

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
        AircraftModelMode m_mode = NotSet; //!< current mode
        bool m_highlightModels = false; //!< highlight if in m_highlightStrings
        QStringList m_highlightStrings; //!< model strings to highlight
        QBrush m_highlightColor { Qt::yellow }; //!< how to highlight
    };
} // ns
#endif // guard
