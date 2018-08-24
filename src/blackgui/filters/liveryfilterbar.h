/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_LIVERYFILTERBAR_H
#define BLACKGUI_FILTERS_LIVERYFILTERBAR_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterwidget.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/rgbcolor.h"

#include <QObject>
#include <QScopedPointer>
#include <memory>

class QWidget;

namespace BlackMisc
{
    namespace Aviation
    {
        class CLivery;
        class CLiveryList;
    }
}
namespace Ui { class CLiveryFilterBar; }
namespace BlackGui
{
    namespace Filters
    {
        /*!
         * Livery filter bar
         */
        class BLACKGUI_EXPORT CLiveryFilterBar :
            public CFilterWidget,
            public BlackGui::Models::IModelFilterProvider<BlackMisc::Aviation::CLiveryList>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLiveryFilterBar(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CLiveryFilterBar() override;

            //! \copydoc Models::IModelFilterProvider::createModelFilter
            virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Aviation::CLiveryList>> createModelFilter() const override;

            //! Filter by livery values
            void filter(const BlackMisc::Aviation::CLivery &livery);

            //! Filter by airline values
            void filter(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao);

        public slots:
            //! \copydoc CFilterWidget::onRowCountChanged
            virtual void onRowCountChanged(int count, bool withFilter) override;

        protected:
            //! Clear form
            virtual void clearForm() override;

        private slots:
            //! Color changed
            void ps_colorChanged(const BlackMisc::CRgbColor &color);

            //! Color distance changed
            void ps_colorDistanceChanged(int distance);

        private:
            QScopedPointer<Ui::CLiveryFilterBar> ui;
        };
    } // ns
} // ns

#endif // guard
