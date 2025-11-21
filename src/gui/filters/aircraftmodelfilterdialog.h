// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_AIRCRAFTMODELFILTERFORM_H
#define SWIFT_GUI_FILTERS_AIRCRAFTMODELFILTERFORM_H

#include <memory>

#include <QObject>
#include <QScopedPointer>

#include "gui/filters/filterdialog.h"
#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"

class QWidget;

namespace swift::misc::simulation
{
    class CAircraftModelList;
}
namespace Ui
{
    class CAircraftModelFilterDialog;
}

namespace swift::gui::filters
{
    //! Form for a aircraft model filter
    class SWIFT_GUI_EXPORT CAircraftModelFilterDialog :
        public CFilterDialog,
        public models::IModelFilterProvider<swift::misc::simulation::CAircraftModelList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelFilterDialog(QWidget *parent = nullptr);

        //! Destructor
        ~CAircraftModelFilterDialog() override;

        //! \copydoc models::IModelFilterProvider::createModelFilter
        std::unique_ptr<swift::gui::models::IModelFilter<swift::misc::simulation::CAircraftModelList>>
        createModelFilter() const override;

    private:
        QScopedPointer<Ui::CAircraftModelFilterDialog> ui;
    };
} // namespace swift::gui::filters

#endif // SWIFT_GUI_FILTERS_AIRCRAFTMODELFILTERFORM_H
