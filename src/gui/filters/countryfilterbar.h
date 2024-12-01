// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_COUNTRYFILTERBAR_H
#define SWIFT_GUI_FILTERS_COUNTRYFILTERBAR_H

#include <memory>

#include <QObject>
#include <QScopedPointer>

#include "gui/filters/filterwidget.h"
#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"

class QWidget;

namespace swift::misc
{
    class CCountryList;
}
namespace Ui
{
    class CCountryFilterBar;
}
namespace swift::gui::filters
{
    /*!
     * Country filter bar
     */
    class SWIFT_GUI_EXPORT CCountryFilterBar :
        public CFilterWidget,
        public models::IModelFilterProvider<swift::misc::CCountryList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCountryFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCountryFilterBar() override;

        //! \copydoc models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<swift::gui::models::IModelFilter<swift::misc::CCountryList>>
        createModelFilter() const override;

    public slots:
        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! Clear form
        virtual void clearForm() override;

    private:
        QScopedPointer<Ui::CCountryFilterBar> ui;
    };
} // namespace swift::gui::filters

#endif // SWIFT_GUI_FILTERS_COUNTRYFILTERBAR_H
