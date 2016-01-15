/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_AIRLINEICAOFILTERBAR_H
#define BLACKGUI_FILTERS_AIRLINEICAOFILTERBAR_H

#include "blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/airlineicaofilter.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CAirlineIcaoFilterBar; }

namespace BlackGui
{
    namespace Filters
    {
        /*!
         * Airline ICAO filter bar
         */
        class BLACKGUI_EXPORT CAirlineIcaoFilterBar :
            public CFilterWidget,
            public BlackGui::Models::IModelFilterProvider<BlackMisc::Aviation::CAirlineIcaoCodeList>,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAirlineIcaoFilterBar(QWidget *parent = nullptr);

            //! Destructor
            ~CAirlineIcaoFilterBar();

            //! \copydoc IModelFilter::getModelFilter
            virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Aviation::CAirlineIcaoCodeList>> createModelFilter() const override;

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

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
} // ns

#endif // guard
