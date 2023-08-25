// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_AIRLINEICAOFILTERBAR_H
#define BLACKGUI_FILTERS_AIRLINEICAOFILTERBAR_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/aviation/airlineicaocode.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

namespace Ui
{
    class CAirlineIcaoFilterBar;
}
namespace BlackMisc
{
    class CCountry;
    namespace Aviation
    {
        class CAirlineIcaoCodeList;
    }
}
namespace BlackGui::Filters
{
    /*!
     * Airline ICAO filter bar
     */
    class BLACKGUI_EXPORT CAirlineIcaoFilterBar :
        public CFilterWidget,
        public Models::IModelFilterProvider<BlackMisc::Aviation::CAirlineIcaoCodeList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoFilterBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAirlineIcaoFilterBar() override;

        //! \copydoc Models::IModelFilterProvider::createModelFilter
        virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Aviation::CAirlineIcaoCodeList>> createModelFilter() const override;

        //! Filter default values by ICAO code
        void filter(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

    public slots:
        //! \copydoc CFilterWidget::onRowCountChanged
        virtual void onRowCountChanged(int count, bool withFilter) override;

    private slots:
        //! Country has been changed
        void ps_CountryChanged(const BlackMisc::CCountry &country);

    protected:
        //! Clear form
        virtual void clearForm() override;

    private:
        QScopedPointer<Ui::CAirlineIcaoFilterBar> ui;
    };
} // ns

#endif // guard
