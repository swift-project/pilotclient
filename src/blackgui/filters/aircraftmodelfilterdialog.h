// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

namespace swift::misc::simulation
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
        public Models::IModelFilterProvider<swift::misc::simulation::CAircraftModelList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelFilterDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelFilterDialog() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<swift::misc::simulation::CAircraftModelList>> createModelFilter() const override;

    private:
        QScopedPointer<Ui::CAircraftModelFilterDialog> ui;
    };
} // namespace

#endif // guard
