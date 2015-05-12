/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodellistmodel.h"
#include "blackmisc/aviation/aircrafticaodata.h"
#include "blackmisc/icons.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CAircraftModelListModel::CAircraftModelListModel(AircraftModelMode mode, QObject *parent) : CListModelBase("CAircraftModelListModel", parent)
        {
            this->setAircraftModelMode(mode);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("CAircraftModelListModel", "callsign");
            (void)QT_TRANSLATE_NOOP("CAircraftModelListModel", "combined type");
            (void)QT_TRANSLATE_NOOP("CAircraftModelListModel", "model");
        }

        void CAircraftModelListModel::setAircraftModelMode(CAircraftModelListModel::AircraftModelMode mode)
        {
            if (this->m_mode == mode) return;
            this->m_mode = mode;
            this->m_columns.clear();
            switch (mode)
            {
            case NotSet:
            case ModelOnly:
                this->m_columns.addColumn(CColumn::standardString("model", { CAircraftModel::IndexModelString}));
                this->m_columns.addColumn(CColumn::standardString("description", { CAircraftModel::IndexDescription}));
                this->m_columns.addColumn(CColumn::standardString("filename", { CAircraftModel::IndexFileName}));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            case MappedModel:
                this->m_columns.addColumn(CColumn::standardValueObject("call", "callsign", CAircraftModel::IndexCallsign));
                this->m_columns.addColumn(CColumn::standardString("model", CAircraftModel::IndexModelString));
                this->m_columns.addColumn(CColumn::standardString("ac", "aircraft ICAO", { CAircraftModel::IndexIcao, CAircraftIcaoData::IndexAircraftDesignator}));
                this->m_columns.addColumn(CColumn::standardString("al", "airline ICAO", { CAircraftModel::IndexIcao, CAircraftIcaoData::IndexAirlineDesignator}));
                // this->m_columns.addColumn(CColumn::standardString("ct", "combined type", { CAircraftModel::IndexIcao, CAircraftIcaoData::IndexCombinedAircraftType}));
                this->m_columns.addColumn(CColumn("q.?", "queried", CAircraftModel::IndexHasQueriedModelString,
                                                  new CBoolIconFormatter(CIcons::StandardIconTick16, CIcons::StandardIconCross16, "queried", "not queried")));
                this->m_columns.addColumn(CColumn::standardString("description",  CAircraftModel::IndexDescription));
                this->m_columns.addColumn(CColumn::standardString("filename",  CAircraftModel::IndexFileName));

                // default sort order
                this->setSortColumnByPropertyIndex(CAircraftModel::IndexModelString);
                this->m_sortOrder = Qt::AscendingOrder;
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }

    } // namespace
} // namespace
