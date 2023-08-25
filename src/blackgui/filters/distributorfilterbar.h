// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_DISTRIBUTORFILTERBAR_H
#define BLACKGUI_FILTERS_DISTRIBUTORFILTERBAR_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

namespace Ui
{
    class CDistributorFilterBar;
}
namespace BlackMisc::Simulation
{
    class CDistributorList;
    class CSimulatorInfo;
}
namespace BlackGui::Filters
{
    /*!
     * Distributor filter bar
     */
    class BLACKGUI_EXPORT CDistributorFilterBar :
        public CFilterWidget,
        public Models::IModelFilterProvider<BlackMisc::Simulation::CDistributorList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDistributorFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDistributorFilterBar() override;

        //! Set simulator
        void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<Models::IModelFilter<BlackMisc::Simulation::CDistributorList>> createModelFilter() const override;

        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! Clear form
        virtual void clearForm() override;

    private:
        //! Trigger filter by changing a value
        void connectTriggerFilterSignals();

        QScopedPointer<Ui::CDistributorFilterBar> ui;
    };
} // ns

#endif // guard
