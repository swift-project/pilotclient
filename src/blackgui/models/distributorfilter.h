/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_DISTRIBUTORFILTER_H
#define BLACKGUI_MODELS_DISTRIBUTORFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/distributorlist.h"

#include <QString>

namespace BlackGui::Models
{
    //! Country filter
    class BLACKGUI_EXPORT CDistributorFilter : public IModelFilter<BlackMisc::Simulation::CDistributorList>
    {
    public:
        //! Constructor
        CDistributorFilter(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! \copydoc IModelFilter::filter
        virtual BlackMisc::Simulation::CDistributorList filter(const BlackMisc::Simulation::CDistributorList &inDistributors) const override;

    private:
        //! Ignore simulator filtering?
        bool ignoreSimulator() const;

        BlackMisc::Simulation::CSimulatorInfo m_simulator;
    };
} // namespace

#endif // guard
