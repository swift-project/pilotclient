// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_DISTRIBUTORFILTER_H
#define SWIFT_GUI_MODELS_DISTRIBUTORFILTER_H

#include "gui/swiftguiexport.h"
#include "gui/models/modelfilter.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/simulation/distributorlist.h"

#include <QString>

namespace swift::gui::models
{
    //! Country filter
    class SWIFT_GUI_EXPORT CDistributorFilter : public IModelFilter<swift::misc::simulation::CDistributorList>
    {
    public:
        //! Constructor
        CDistributorFilter(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! \copydoc IModelFilter::filter
        virtual swift::misc::simulation::CDistributorList filter(const swift::misc::simulation::CDistributorList &inDistributors) const override;

    private:
        //! Ignore simulator filtering?
        bool ignoreSimulator() const;

        swift::misc::simulation::CSimulatorInfo m_simulator;
    };
} // namespace

#endif // guard
