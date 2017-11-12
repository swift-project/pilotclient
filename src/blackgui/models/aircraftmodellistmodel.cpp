/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/orderable.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/timestampbased.h"

#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Models
    {
        CAircraftModelListModel::CAircraftModelListModel(AircraftModelMode mode, QObject *parent) :
            COrderableListModelDbObjects("CAircraftModelListModel", parent)
        {
            this->setAircraftModelMode(mode);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("CAircraftModelListModel", "callsign");
            (void)QT_TRANSLATE_NOOP("CAircraftModelListModel", "combined type");
            (void)QT_TRANSLATE_NOOP("CAircraftModelListModel", "model");
        }

        void CAircraftModelListModel::setAircraftModelMode(CAircraftModelListModel::AircraftModelMode mode)
        {
            if (this->m_mode == mode) { return; }
            this->m_mode = mode;
            this->m_columns.clear();
            switch (mode)
            {
            case NotSet:
            case OwnAircraftModelClient:
                this->m_columns.addColumn(CColumn::standardString("model", { CAircraftModel::IndexModelString}));
                this->m_columns.addColumn(CColumn("DB", "DB metadata", CAircraftModel::IndexDatabaseIcon, new CPixmapFormatter()));
                this->m_columns.addColumn(CColumn::standardString("DB", "parts from DB", { CAircraftModel::IndexMembersDbStatus}));
                this->m_columns.addColumn(CColumn("mode", "model mode(include, exclude)", CAircraftModel::IndexModelModeAsIcon, new CPixmapFormatter()));
                this->m_columns.addColumn(CColumn::standardString("description", { CAircraftModel::IndexDescription}));
                this->m_columns.addColumn(CColumn::standardString("sim.", "simulator supported", CAircraftModel::IndexSimulatorInfoAsString));

                this->m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey}));

                this->m_columns.addColumn(CColumn::standardString("aircraft", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexDesignatorManufacturer}));
                this->m_columns.addColumn(CColumn::standardString("livery", { CAircraftModel::IndexLivery, CLivery::IndexCombinedCode}));
                this->m_columns.addColumn(CColumn::standardString("airline", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexDesignatorNameCountry}));

                this->m_columns.addColumn(CColumn::standardString("name", { CAircraftModel::IndexName}));
                this->m_columns.addColumn(CColumn::standardString("filename", { CAircraftModel::IndexFileName}));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            case OwnModelSet:
                // intentional fall thru
                this->m_columns.addColumn(CColumn::orderColumn());

            case OwnAircraftModelMappingTool:
            case StashModel:
                this->m_columns.addColumn(CColumn::standardString("model", CAircraftModel::IndexModelString));
                this->m_columns.addColumn(CColumn("DB", "DB metadata", CAircraftModel::IndexDatabaseIcon, new CPixmapFormatter()));
                if (mode == StashModel)
                {
                    this->m_columns.addColumn(CColumn::standardString("flags", "parts from DB", { CAircraftModel::IndexMembersDbStatus}));
                }
                this->m_columns.addColumn(CColumn("mode", "model mode(include, exclude)", CAircraftModel::IndexModelModeAsIcon, new CPixmapFormatter()));
                this->m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey}));
                if (mode == OwnModelSet)
                {
                    this->m_columns.addColumn(CColumn::standardString("d#", "distributor order", { CAircraftModel::IndexDistributor, CDistributor::IndexOrderString}));
                }
                this->m_columns.addColumn(CColumn::standardString("ac", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexAircraftDesignator}));
                this->m_columns.addColumn(CColumn::standardString("fam.", "aircraft family", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexFamily}));
                this->m_columns.addColumn(CColumn::standardString("livery", { CAircraftModel::IndexLivery, CLivery::IndexCombinedCode}));
                this->m_columns.addColumn(CColumn::standardString("al", "airline ICAO", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator}));
                this->m_columns.addColumn(CColumn::standardString("description",  CAircraftModel::IndexDescription));
                this->m_columns.addColumn(CColumn::standardString("filename",  CAircraftModel::IndexFileName));
                this->m_columns.addColumn(CColumn::standardString("file ts.", "file timestamp", CAircraftModel::IndexFileTimestampFormattedYmdhms));
                this->m_columns.addColumn(CColumn::standardString("icon",  CAircraftModel::IndexIconPath));
                this->m_columns.addColumn(CColumn::standardString("changed", CAircraftModel::IndexUtcTimestampFormattedYmdhms));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            case Database:
                this->m_columns.addColumn(CColumn::standardString("id", CAircraftModel::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
                this->m_columns.addColumn(CColumn::standardString("model", CAircraftModel::IndexModelString));
                this->m_columns.addColumn(CColumn("mode", "model mode(include, exclude)", CAircraftModel::IndexModelModeAsIcon, new CPixmapFormatter()));

                this->m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey}));
                this->m_columns.addColumn(CColumn::standardString("name", CAircraftModel::IndexName));
                this->m_columns.addColumn(CColumn::standardString("description", CAircraftModel::IndexDescription));
                this->m_columns.addColumn(CColumn::standardString("sim.", "simulator supported", CAircraftModel::IndexSimulatorInfoAsString));

                this->m_columns.addColumn(CColumn::standardString("ac", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexAircraftDesignator}));
                this->m_columns.addColumn(CColumn::standardString("fam.", "aircraft family", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexFamily}));
                this->m_columns.addColumn(CColumn::standardString("manufacturer", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexManufacturer}));

                this->m_columns.addColumn(CColumn::standardString("code", { CAircraftModel::IndexLivery, CLivery::IndexCombinedCode}));
                this->m_columns.addColumn(CColumn::standardString("liv.desc.", "livery description", { CAircraftModel::IndexLivery, CLivery::IndexDescription}));
                this->m_columns.addColumn(CColumn::standardString("al", "airline ICAO", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator}));
                this->m_columns.addColumn(CColumn::standardString("al.name", "airline name", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineName }));

                this->m_columns.addColumn(CColumn("fuse.", "fuselage color", { CAircraftModel::IndexLivery, CLivery::IndexColorFuselage }, new CColorFormatter()));
                this->m_columns.addColumn(CColumn("tail", "tail color", { CAircraftModel::IndexLivery, CLivery::IndexColorTail }, new CColorFormatter()));
                this->m_columns.addColumn(CColumn("mil.", "military livery", { CAircraftModel::IndexLivery, CLivery::IndexIsMilitary}, new CBoolIconFormatter("military", "civil")));
                this->m_columns.addColumn(CColumn::standardString("changed", CAircraftModel::IndexUtcTimestampFormattedYmdhms));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            case VPilotRuleModel:
                this->m_columns.addColumn(CColumn::standardString("model", CAircraftModel::IndexModelString));
                this->m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey}));
                this->m_columns.addColumn(CColumn::standardString("sim.", "simulator supported", CAircraftModel::IndexSimulatorInfoAsString));

                this->m_columns.addColumn(CColumn::standardString("ac", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexAircraftDesignator}));

                this->m_columns.addColumn(CColumn::standardString("al", "airline ICAO", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator}));
                this->m_columns.addColumn(CColumn::standardString("al.name", "airline name", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineName }));

                this->m_columns.addColumn(CColumn::standardString("changed", CAircraftModel::IndexUtcTimestampFormattedYmdhms));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }

        void CAircraftModelListModel::setHighlightModelStrings(const QStringList &modelStrings)
        {
            this->beginResetModel();
            m_highlightStrings = modelStrings;
            this->endResetModel();
        }

        void CAircraftModelListModel::setHighlightModelStrings(bool highlightModelStrings)
        {
            if (m_highlightModelStrings == highlightModelStrings) { return; }

            this->beginResetModel();
            m_highlightModelStrings = highlightModelStrings;
            this->endResetModel();
        }

        QStringList CAircraftModelListModel::getModelStrings(bool sort) const
        {
            if (this->isEmpty()) { return QStringList(); }
            return this->container().getModelStringList(sort);
        }

        void CAircraftModelListModel::replaceOrAddByModelString(const CAircraftModelList &models)
        {
            if (models.isEmpty()) { return; }
            CAircraftModelList currentModels(container());
            currentModels.removeModelsWithString(models.getModelStringList(true), Qt::CaseInsensitive);
            currentModels.push_back(models);
            this->updateContainerMaybeAsync(currentModels);
        }

        QVariant CAircraftModelListModel::data(const QModelIndex &index, int role) const
        {
            if (role == Qt::BackgroundRole)
            {
                const bool ms = highlightModelStrings() && !m_highlightStrings.isEmpty();
                if (!ms) { return CListModelDbObjects::data(index, role); }

                // the underlying model object
                const CAircraftModel model(this->at(index));

                // highlight stashed first
                if (m_highlightStrings.contains(model.getModelString(), Qt::CaseInsensitive))
                {
                    return this->m_highlightColor;
                }

                return QVariant();
            }
            else if (role == Qt::ToolTipRole)
            {
                // the underlying model object as summary
                const CAircraftModel model(this->at(index));
                return model.asHtmlSummary("<br>");
            }
            return CListModelDbObjects::data(index, role);
        }

        void CAircraftModelListModel::clearHighlighting()
        {
            this->m_highlightModelStrings = false;
            this->m_highlightStrings.clear();
            COrderableListModelDbObjects::clearHighlighting();
        }
        bool CAircraftModelListModel::hasHighlightedRows() const
        {
            return !m_highlightStrings.isEmpty();
        }
    } // namespace
} // namespace
