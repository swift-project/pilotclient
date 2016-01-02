/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_COUNTRYFILTERBAR_H
#define BLACKGUI_FILTERS_COUNTRYFILTERBAR_H

#include "blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/countryfilter.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QScopedPointer>

namespace Ui { class CCountryFilterBar; }

namespace BlackGui
{
    namespace Filters
    {
        /*!
         * Country filter bar
         */
        class BLACKGUI_EXPORT CCountryFilterBar :
            public CFilterWidget,
            public BlackGui::Models::IModelFilterProvider<BlackMisc::CCountryList>,
            public BlackMisc::Network::CWebDataServicesAware
        {
        public:
            //! Constructor
            explicit CCountryFilterBar(QWidget *parent = nullptr);

            //! Destructor
            ~CCountryFilterBar();

            //! \copydoc IModelFilter::getModelFilter
            virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::CCountryList>> createModelFilter() const override;

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

        public slots:
            //! \copydoc CFilterWidget::onRowCountChanged
            virtual void onRowCountChanged(int count, bool withFilter) override;

        protected:
            //! Clear form
            virtual void clearForm() override;

        private:
            QScopedPointer<Ui::CCountryFilterBar> ui;
        };
    }
}

#endif // guard
