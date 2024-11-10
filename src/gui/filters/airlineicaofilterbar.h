// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_AIRLINEICAOFILTERBAR_H
#define SWIFT_GUI_FILTERS_AIRLINEICAOFILTERBAR_H

#include "gui/swiftguiexport.h"
#include "gui/filters/filterwidget.h"
#include "gui/models/modelfilter.h"
#include "misc/aviation/airlineicaocode.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

namespace Ui
{
    class CAirlineIcaoFilterBar;
}
namespace swift::misc
{
    class CCountry;
    namespace aviation
    {
        class CAirlineIcaoCodeList;
    }
}
namespace swift::gui::filters
{
    /*!
     * Airline ICAO filter bar
     */
    class SWIFT_GUI_EXPORT CAirlineIcaoFilterBar :
        public CFilterWidget,
        public models::IModelFilterProvider<swift::misc::aviation::CAirlineIcaoCodeList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAirlineIcaoFilterBar() override;

        //! \copydoc models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<swift::gui::models::IModelFilter<swift::misc::aviation::CAirlineIcaoCodeList>> createModelFilter() const override;

        //! Filter default values by ICAO code
        void filter(const swift::misc::aviation::CAirlineIcaoCode &icao);

    public slots:
        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    private slots:
        //! Country has been changed
        void ps_CountryChanged(const swift::misc::CCountry &country);

    protected:
        //! Clear form
        virtual void clearForm() override;

    private:
        QScopedPointer<Ui::CAirlineIcaoFilterBar> ui;
    };
} // ns

#endif // guard
