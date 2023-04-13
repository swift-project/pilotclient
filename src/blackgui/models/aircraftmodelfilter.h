/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTMODELFILTER_H
#define BLACKGUI_MODELS_AIRCRAFTMODELFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/db/datastore.h"

#include <QString>

namespace BlackGui::Models
{
    //! Filter for aircraft models
    class BLACKGUI_EXPORT CAircraftModelFilter : public IModelFilter<BlackMisc::Simulation::CAircraftModelList>
    {
    public:
        //! Constructor
        CAircraftModelFilter(int id,
                             const QString &modelKey, const QString &description,
                             BlackMisc::Simulation::CAircraftModel::ModelModeFilter modelMode,
                             BlackMisc::Db::DbKeyStateFilter dbKeyFilter,
                             Qt::CheckState military, Qt::CheckState colorLiveries,
                             const QString &aircraftIcao, const QString &aircraftManufacturer,
                             const QString &airlineIcao, const QString &airlineName,
                             const QString &liveryCode,
                             const QString &fileName,
                             const QString &combinedType,
                             const BlackMisc::Simulation::CSimulatorInfo &simInfo = BlackMisc::Simulation::CSimulatorInfo::allSimulators(),
                             const BlackMisc::Simulation::CDistributor &distributor = BlackMisc::Simulation::CDistributor());

        //! \copydoc IModelFilter::filter
        virtual BlackMisc::Simulation::CAircraftModelList filter(const BlackMisc::Simulation::CAircraftModelList &inContainer) const override;

    private:
        int m_id = -1;
        QString m_modelKey;
        QString m_description;
        BlackMisc::Simulation::CAircraftModel::ModelModeFilter m_modelMode;
        BlackMisc::Db::DbKeyStateFilter m_dbKeyFilter;
        Qt::CheckState m_military;
        Qt::CheckState m_colorLiveries;
        QString m_aircraftIcao;
        QString m_aircraftManufacturer;
        QString m_airlineIcao;
        QString m_airlineName;
        QString m_liveryCode;
        QString m_fileName;
        QString m_combinedType;
        BlackMisc::Simulation::CSimulatorInfo m_simulatorInfo;
        BlackMisc::Simulation::CDistributor m_distributor;
        bool valid() const;
    };
} // namespace

#endif // guard
