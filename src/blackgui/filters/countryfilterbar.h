/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

namespace BlackMisc
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
        public Models::IModelFilterProvider<BlackMisc::CCountryList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCountryFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCountryFilterBar() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::CCountryList>> createModelFilter() const override;

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
