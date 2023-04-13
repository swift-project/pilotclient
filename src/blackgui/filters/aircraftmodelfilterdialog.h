/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_AIRCRAFTMODELFILTERFORM_H
#define BLACKGUI_FILTERS_AIRCRAFTMODELFILTERFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterdialog.h"
#include "blackgui/models/modelfilter.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

class QWidget;

namespace BlackMisc::Simulation
{
    class CAircraftModelList;
}
namespace Ui
{
    class CAircraftModelFilterDialog;
}

namespace BlackGui::Filters
{
    //! Form for a aircraft model filter
    class BLACKGUI_EXPORT CAircraftModelFilterDialog :
        public CFilterDialog,
        public Models::IModelFilterProvider<BlackMisc::Simulation::CAircraftModelList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelFilterDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelFilterDialog() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Simulation::CAircraftModelList>> createModelFilter() const override;

    private:
        QScopedPointer<Ui::CAircraftModelFilterDialog> ui;
    };
} // namespace

#endif // guard
