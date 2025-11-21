// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTMODELLISTMODEL_H
#define SWIFT_GUI_MODELS_AIRCRAFTMODELLISTMODEL_H

#include <QBrush>
#include <QStringList>
#include <QVariant>
#include <Qt>

#include "gui/models/listmodeldbobjects.h"
#include "gui/swiftguiexport.h"
#include "misc/simulation/aircraftmodellist.h"

class QModelIndex;

namespace swift::misc::simulation
{
    class CAircraftModel;
}
namespace swift::gui::models
{
    //! Aircraft model list model
    class SWIFT_GUI_EXPORT CAircraftModelListModel :
        public COrderableListModelDbObjects<swift::misc::simulation::CAircraftModelList, int, true>
    {
        Q_OBJECT

    public:
        //! How to display
        //! \sa swift::misc::simulation::CAircraftModel::ModelType
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
        ~CAircraftModelListModel() override = default;

        //! Mode
        void setAircraftModelMode(CAircraftModelListModel::AircraftModelMode stationMode);

        //! Mode
        AircraftModelMode getModelMode() const { return m_mode; }

        //! Highlight models
        void setHighlightModelStrings(const QStringList &modelStrings = QStringList());

        //! Highlight models
        void setHighlightModels(const swift::misc::simulation::CAircraftModelList &highlightModels);

        //! Highlight models
        bool highlightModels() const { return m_highlightModels; }

        //! Highlight models
        void setHighlight(bool highlightModels);

        //! The highlight color
        void setHighlightColor(const QBrush &brush) { m_highlightColor = brush; }

        //! \copydoc CListModelBaseNonTemplate::clearHighlighting
        void clearHighlighting() override;

        //! \copydoc CListModelBaseNonTemplate::hasHighlightedRows
        bool hasHighlightedRows() const override;

        //! Model strings
        QStringList getModelStrings(bool sort) const;

        //! Replace models with same model string, or just add
        void replaceOrAddByModelString(const swift::misc::simulation::CAircraftModelList &models);

        //! \copydoc QAbstractItemModel::data
        QVariant data(const QModelIndex &index, int role) const override;

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::isOrderable
        bool isOrderable() const override { return true; }

    private:
        AircraftModelMode m_mode = NotSet; //!< current mode
        bool m_highlightModels = false; //!< highlight if in m_highlightStrings
        QStringList m_highlightStrings; //!< model strings to highlight
        QBrush m_highlightColor { Qt::yellow }; //!< how to highlight
    };
} // namespace swift::gui::models
#endif // SWIFT_GUI_MODELS_AIRCRAFTMODELLISTMODEL_H
