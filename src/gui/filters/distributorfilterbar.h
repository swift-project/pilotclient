// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_DISTRIBUTORFILTERBAR_H
#define SWIFT_GUI_FILTERS_DISTRIBUTORFILTERBAR_H

#include <memory>

#include <QObject>
#include <QScopedPointer>

#include "gui/filters/filterwidget.h"
#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"

namespace Ui
{
    class CDistributorFilterBar;
}
namespace swift::misc::simulation
{
    class CDistributorList;
    class CSimulatorInfo;
} // namespace swift::misc::simulation
namespace swift::gui::filters
{
    /*!
     * Distributor filter bar
     */
    class SWIFT_GUI_EXPORT CDistributorFilterBar :
        public CFilterWidget,
        public models::IModelFilterProvider<swift::misc::simulation::CDistributorList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDistributorFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDistributorFilterBar() override;

        //! Set simulator
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! \copydoc models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<models::IModelFilter<swift::misc::simulation::CDistributorList>>
        createModelFilter() const override;

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
} // namespace swift::gui::filters

#endif // guard
