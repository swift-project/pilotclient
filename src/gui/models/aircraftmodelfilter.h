// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTMODELFILTER_H
#define SWIFT_GUI_MODELS_AIRCRAFTMODELFILTER_H

#include <QString>

#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"
#include "misc/db/datastore.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/simulatorinfo.h"

namespace swift::gui::models
{
    //! Filter for aircraft models
    class SWIFT_GUI_EXPORT CAircraftModelFilter : public IModelFilter<swift::misc::simulation::CAircraftModelList>
    {
    public:
        //! Constructor
        CAircraftModelFilter(
            int id, const QString &modelKey, const QString &description,
            swift::misc::simulation::CAircraftModel::ModelModeFilter modelMode,
            swift::misc::db::DbKeyStateFilter dbKeyFilter, Qt::CheckState military, Qt::CheckState colorLiveries,
            const QString &aircraftIcao, const QString &aircraftManufacturer, const QString &airlineIcao,
            const QString &airlineName, const QString &liveryCode, const QString &fileName, const QString &combinedType,
            const swift::misc::simulation::CSimulatorInfo &simInfo =
                swift::misc::simulation::CSimulatorInfo::allSimulators(),
            const swift::misc::simulation::CDistributor &distributor = swift::misc::simulation::CDistributor());

        //! \copydoc IModelFilter::filter
        swift::misc::simulation::CAircraftModelList
        filter(const swift::misc::simulation::CAircraftModelList &inContainer) const override;

    private:
        int m_id = -1;
        QString m_modelKey;
        QString m_description;
        swift::misc::simulation::CAircraftModel::ModelModeFilter m_modelMode;
        swift::misc::db::DbKeyStateFilter m_dbKeyFilter;
        Qt::CheckState m_military;
        Qt::CheckState m_colorLiveries;
        QString m_aircraftIcao;
        QString m_aircraftManufacturer;
        QString m_airlineIcao;
        QString m_airlineName;
        QString m_liveryCode;
        QString m_fileName;
        QString m_combinedType;
        swift::misc::simulation::CSimulatorInfo m_simulatorInfo;
        swift::misc::simulation::CDistributor m_distributor;
        bool valid() const;
    };
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_AIRCRAFTMODELFILTER_H
