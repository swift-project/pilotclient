// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_FILTERBARAIRCRAFTMODEL_H
#define BLACKGUI_FILTERS_FILTERBARAIRCRAFTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

namespace Ui
{
    class CAircraftModelFilterBar;
}
namespace BlackMisc::Simulation
{
    class CAircraftModelList;
}
namespace BlackGui::Filters
{
    /*!
     * Filter bar for aircraft models
     */
    class BLACKGUI_EXPORT CAircraftModelFilterBar :
        public CFilterWidget,
        public Models::IModelFilterProvider<BlackMisc::Simulation::CAircraftModelList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelFilterBar() override;

        //! Show count
        void displayCount(bool show);

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<Models::IModelFilter<BlackMisc::Simulation::CAircraftModelList>> createModelFilter() const override;

        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! \copydoc CFilterWidget::clearForm
        virtual void clearForm() override;

    private:
        //! Simulator selection changed
        void onSimulatorSelectionChanged(const BlackMisc::Simulation::CSimulatorInfo &info);

        //! Distributor changed
        void onDistributorChanged(const BlackMisc::Simulation::CDistributor &distributor);

        //! Combined type changed
        void onCombinedTypeChanged(const QString &combinedType);

        //! Checkbox has been changed
        void onCheckBoxChanged(bool state);

        //! Trigger filter by changing a value
        void connectTriggerFilterSignals();

        QScopedPointer<Ui::CAircraftModelFilterBar> ui;
    };
} // ns

#endif // guard
