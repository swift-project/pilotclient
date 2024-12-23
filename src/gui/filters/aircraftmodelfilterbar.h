// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_FILTERBARAIRCRAFTMODEL_H
#define SWIFT_GUI_FILTERS_FILTERBARAIRCRAFTMODEL_H

#include <memory>

#include <QObject>
#include <QScopedPointer>

#include "gui/filters/filterwidget.h"
#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/simulatorinfo.h"

namespace Ui
{
    class CAircraftModelFilterBar;
}
namespace swift::misc::simulation
{
    class CAircraftModelList;
}
namespace swift::gui::filters
{
    /*!
     * Filter bar for aircraft models
     */
    class SWIFT_GUI_EXPORT CAircraftModelFilterBar :
        public CFilterWidget,
        public models::IModelFilterProvider<swift::misc::simulation::CAircraftModelList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelFilterBar() override;

        //! Show count
        void displayCount(bool show);

        //! \copydoc models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<models::IModelFilter<swift::misc::simulation::CAircraftModelList>>
        createModelFilter() const override;

        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! \copydoc CFilterWidget::clearForm
        virtual void clearForm() override;

    private:
        //! Simulator selection changed
        void onSimulatorSelectionChanged(const swift::misc::simulation::CSimulatorInfo &info);

        //! Distributor changed
        void onDistributorChanged(const swift::misc::simulation::CDistributor &distributor);

        //! Combined type changed
        void onCombinedTypeChanged(const QString &combinedType);

        //! Checkbox has been changed
        void onCheckBoxChanged(bool state);

        //! Trigger filter by changing a value
        void connectTriggerFilterSignals();

        QScopedPointer<Ui::CAircraftModelFilterBar> ui;
    };
} // namespace swift::gui::filters

#endif // SWIFT_GUI_FILTERS_FILTERBARAIRCRAFTMODEL_H
