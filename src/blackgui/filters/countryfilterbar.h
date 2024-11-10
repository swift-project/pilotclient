// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_COUNTRYFILTERBAR_H
#define BLACKGUI_FILTERS_COUNTRYFILTERBAR_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

class QWidget;

namespace swift::misc
{
    class CCountryList;
}
namespace Ui
{
    class CCountryFilterBar;
}
namespace BlackGui::Filters
{
    /*!
     * Country filter bar
     */
    class BLACKGUI_EXPORT CCountryFilterBar :
        public CFilterWidget,
        public Models::IModelFilterProvider<swift::misc::CCountryList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCountryFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCountryFilterBar() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<swift::misc::CCountryList>> createModelFilter() const override;

    public slots:
        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    protected:
        //! Clear form
        virtual void clearForm() override;

    private:
        QScopedPointer<Ui::CCountryFilterBar> ui;
    };
} // ns

#endif // guard
