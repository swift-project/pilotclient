/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTMODELFILTER_H
#define BLACKGUI_AIRCRAFTMODELFILTER_H

#include "blackgui/blackguiexport.h"
#include "modelfilter.h"
#include "blackmisc/simulation/aircraftmodellist.h"

namespace BlackGui
{
    namespace Models
    {
        //! Filter for aircraft models
        class BLACKGUI_EXPORT CAircraftModelFilter : public IModelFilter<BlackMisc::Simulation::CAircraftModelList>
        {
        public:
            //! Constructor
            CAircraftModelFilter(const QString &modelKey, const QString &description, BlackMisc::Simulation::CAircraftModel::ModelModeFilter modelMode,
                                 const QString &aircraftIcao, const QString &aircraftManufacturer,
                                 const QString &airlineIcao, const QString &airlineName,
                                 const QString &liveryCode,
                                 const BlackMisc::Simulation::CSimulatorInfo &simInfo = BlackMisc::Simulation::CSimulatorInfo::allSimulators(),
                                 const BlackMisc::Simulation::CDistributor &distributor = BlackMisc::Simulation::CDistributor()
                                );

            //! \copydoc IModelFilter::filter
            virtual BlackMisc::Simulation::CAircraftModelList filter(const BlackMisc::Simulation::CAircraftModelList &inContainer) const override;

        private:
            QString m_modelKey;
            QString m_description;
            BlackMisc::Simulation::CAircraftModel::ModelModeFilter m_modelMode;
            QString m_aircraftIcao;
            QString m_aircraftManufacturer;
            QString m_airlineIcao;
            QString m_airlineName;
            QString m_liveryCode;
            BlackMisc::Simulation::CSimulatorInfo m_simulatorInfo;
            BlackMisc::Simulation::CDistributor   m_distributor;
            bool valid() const;
        };
    } // namespace
} // namespace

#endif // guard
