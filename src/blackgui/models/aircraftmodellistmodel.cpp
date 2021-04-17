/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
using namespace BlackMisc::PhysicalQuantities;

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
            if (m_mode == mode) { return; }
            m_mode = mode;
            m_columns.clear();
            switch (mode)
            {
            case NotSet:
            case OwnAircraftModelClient:
                m_columns.addColumn(CColumn::standardString("model", { CAircraftModel::IndexModelString}));
                m_columns.addColumn(CColumn("DB", "DB metadata", CAircraftModel::IndexDatabaseIcon, new CPixmapFormatter()));
                m_columns.addColumn(CColumn::standardString("DB", "parts from DB", { CAircraftModel::IndexMembersDbStatus }));
                m_columns.addColumn(CColumn("mode", "model mode(include, exclude)", CAircraftModel::IndexModelModeAsIcon, new CPixmapFormatter()));
                m_columns.addColumn(CColumn::standardString("description", { CAircraftModel::IndexDescription }));
                m_columns.addColumn(CColumn::standardString("sim.", "simulator supported", CAircraftModel::IndexSimulatorInfoAsString));

                m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey }));

                m_columns.addColumn(CColumn::standardString("aircraft", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexDesignatorManufacturer }));
                m_columns.addColumn(CColumn::standardString("livery", { CAircraftModel::IndexLivery, CLivery::IndexCombinedCode}));
                m_columns.addColumn(CColumn::standardString("airline", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexDesignatorNameCountry }));

                m_columns.addColumn(CColumn::standardString("name", { CAircraftModel::IndexName}));
                m_columns.addColumn(CColumn::standardString("filename", { CAircraftModel::IndexFileName}));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                m_sortOrder = Qt::AscendingOrder;
                break;

            case OwnModelSet:
                // intentional fall thru
                m_columns.addColumn(CColumn::orderColumn());
                [[fallthrough]];

            case OwnAircraftModelMappingTool:
            case StashModel:
                m_columns.addColumn(CColumn::standardString("model", CAircraftModel::IndexModelString));
                m_columns.addColumn(CColumn("DB", "DB metadata", CAircraftModel::IndexDatabaseIcon, new CPixmapFormatter()));
                if (mode == StashModel)
                {
                    m_columns.addColumn(CColumn::standardString("flags", "parts from DB", { CAircraftModel::IndexMembersDbStatus}));
                }
                m_columns.addColumn(CColumn("mode", "model mode(include, exclude)", CAircraftModel::IndexModelModeAsIcon, new CPixmapFormatter()));
                m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey }));
                if (mode == OwnModelSet)
                {
                    m_columns.addColumn(CColumn::standardString("d#", "distributor order", { CAircraftModel::IndexDistributor, CDistributor::IndexOrderString }));
                }
                m_columns.addColumn(CColumn::standardString("ac", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexAircraftDesignator }));
                m_columns.addColumn(CColumn::standardString("fam.", "aircraft family", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexFamily }));
                m_columns.addColumn(CColumn::standardString("cat.", "category", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexCategory, CAircraftCategory::IndexLevelStringAndPath }));
                m_columns.addColumn(CColumn::standardString("livery", { CAircraftModel::IndexLivery, CLivery::IndexCombinedCode}));
                m_columns.addColumn(CColumn::standardString("al", "airline ICAO", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator }));
                m_columns.addColumn(CColumn("CG", CAircraftModel::IndexCG, new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::ft(), 1)));
                m_columns.addColumn(CColumn::standardString("parts",  CAircraftModel::IndexSupportedParts));
                m_columns.addColumn(CColumn::standardString("description",  CAircraftModel::IndexDescription));
                m_columns.addColumn(CColumn::standardString("filename",  CAircraftModel::IndexFileName));
                m_columns.addColumn(CColumn::standardString("file ts.", "file timestamp", CAircraftModel::IndexFileTimestampFormattedYmdhms));
                m_columns.addColumn(CColumn::standardString("icon",  CAircraftModel::IndexIconPath));
                m_columns.addColumn(CColumn::standardString("version",  CAircraftModel::IndexVersion));
                m_columns.addColumn(CColumn::standardString("changed", CAircraftModel::IndexUtcTimestampFormattedYmdhms));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                m_sortOrder = Qt::AscendingOrder;
                break;

            case Database:
                m_columns.addColumn(CColumn::standardString("id", CAircraftModel::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
                m_columns.addColumn(CColumn::standardString("model", CAircraftModel::IndexAllModelStrings));
                m_columns.addColumn(CColumn("mode", "model mode(include, exclude)", CAircraftModel::IndexModelModeAsIcon, new CPixmapFormatter()));

                m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey}));
                m_columns.addColumn(CColumn::standardString("name", CAircraftModel::IndexName));
                m_columns.addColumn(CColumn::standardString("description", CAircraftModel::IndexDescription));
                m_columns.addColumn(CColumn::standardString("sim.", "simulator supported", CAircraftModel::IndexSimulatorInfoAsString));

                m_columns.addColumn(CColumn::standardString("ac", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexAircraftDesignator }));
                m_columns.addColumn(CColumn::standardString("fam.", "aircraft family", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexFamily }));
                m_columns.addColumn(CColumn::standardString("cat.", "category", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexCategory, CAircraftCategory::IndexPath }));
                m_columns.addColumn(CColumn::standardString("manufacturer", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexManufacturer }));

                m_columns.addColumn(CColumn::standardString("code", { CAircraftModel::IndexLivery, CLivery::IndexCombinedCode}));
                m_columns.addColumn(CColumn::standardString("liv.desc.", "livery description", { CAircraftModel::IndexLivery, CLivery::IndexDescription}));
                m_columns.addColumn(CColumn::standardString("al", "airline ICAO", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator }));
                m_columns.addColumn(CColumn::standardString("al.name", "airline name", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineName }));

                m_columns.addColumn(CColumn("CG", CAircraftModel::IndexCG, new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::ft(), 1)));
                m_columns.addColumn(CColumn::standardString("parts",  CAircraftModel::IndexSupportedParts));

                m_columns.addColumn(CColumn("fuse.", "fuselage color", { CAircraftModel::IndexLivery, CLivery::IndexColorFuselage }, new CColorFormatter()));
                m_columns.addColumn(CColumn("tail", "tail color", { CAircraftModel::IndexLivery, CLivery::IndexColorTail }, new CColorFormatter()));
                m_columns.addColumn(CColumn("mil.", "military livery", { CAircraftModel::IndexLivery, CLivery::IndexIsMilitary }, new CBoolIconFormatter("military", "civil")));
                m_columns.addColumn(CColumn::standardString("version",  CAircraftModel::IndexVersion));
                m_columns.addColumn(CColumn::standardString("changed", CAircraftModel::IndexUtcTimestampFormattedYmdhms));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                m_sortOrder = Qt::AscendingOrder;
                break;

            case VPilotRuleModel:
                m_columns.addColumn(CColumn::standardString("model", CAircraftModel::IndexModelString));
                m_columns.addColumn(CColumn::standardString("dist.", "distributor", { CAircraftModel::IndexDistributor, CDistributor::IndexDbStringKey}));
                m_columns.addColumn(CColumn::standardString("sim.", "simulator supported", CAircraftModel::IndexSimulatorInfoAsString));

                m_columns.addColumn(CColumn::standardString("ac", "aircraft ICAO", { CAircraftModel::IndexAircraftIcaoCode, CAircraftIcaoCode::IndexAircraftDesignator}));

                m_columns.addColumn(CColumn::standardString("al", "airline ICAO", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator}));
                m_columns.addColumn(CColumn::standardString("al.name", "airline name", { CAircraftModel::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineName }));

                m_columns.addColumn(CColumn::standardString("changed", CAircraftModel::IndexUtcTimestampFormattedYmdhms));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                m_sortOrder = Qt::AscendingOrder;
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

        void CAircraftModelListModel::setHighlightModels(const CAircraftModelList &highlightModels)
        {
            this->setHighlightModelStrings(highlightModels.getModelStringList(true));
        }

        void CAircraftModelListModel::setHighlight(bool highlightModels)
        {
            if (m_highlightModels == highlightModels) { return; }

            this->beginResetModel();
            m_highlightModels = highlightModels;
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
                const bool ms = highlightModels() && !m_highlightStrings.isEmpty();
                if (!ms) { return CListModelDbObjects::data(index, role); }

                // the underlying model object
                const CAircraftModel model(this->at(index));

                // highlight stashed first
                if (m_highlightStrings.contains(model.getModelString(), Qt::CaseInsensitive))
                {
                    return m_highlightColor;
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
            m_highlightModels = false;
            m_highlightStrings.clear();
            COrderableListModelDbObjects::clearHighlighting();
        }

        bool CAircraftModelListModel::hasHighlightedRows() const
        {
            return !m_highlightStrings.isEmpty();
        }
    } // namespace
} // namespace
