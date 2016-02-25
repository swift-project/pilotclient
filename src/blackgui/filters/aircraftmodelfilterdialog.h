/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_AIRCRAFTMODELFILTERFORM_H
#define BLACKGUI_FILTERS_AIRCRAFTMODELFILTERFORM_H

#include "blackgui/blackguiexport.h"
#include "filterdialog.h"
#include "blackgui/models/aircraftmodelfilter.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CAircraftModelFilterDialog; }

namespace BlackGui
{
    namespace Filters
    {
        //! Form for a aircraft model filter
        class BLACKGUI_EXPORT CAircraftModelFilterDialog :
            public CFilterDialog,
            public BlackGui::Models::IModelFilterProvider<BlackMisc::Simulation::CAircraftModelList>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftModelFilterDialog(QWidget *parent = nullptr);

            //! Destructor
            ~CAircraftModelFilterDialog();

            //! \copydoc Models::IModelFilterProvider::createModelFilter
            virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Simulation::CAircraftModelList>> createModelFilter() const override;

        private:
            QScopedPointer<Ui::CAircraftModelFilterDialog> ui;
        };

    } // namespace
} // namespace

#endif // guard
