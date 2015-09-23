/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_FILTERWIDGET_H
#define BLACKGUI_FILTERS_FILTERWIDGET_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterbarbuttons.h"
#include <QFrame>

namespace BlackGui
{
    namespace Filters
    {
        //! Base for filter dialog
        class BLACKGUI_EXPORT CFilterWidget : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            CFilterWidget(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CFilterWidget();

        public slots:
            //! Row count has been changed
            virtual void onRowCountChanged(int count, bool withFilter) = 0;

            //! Trigger filtering
            virtual void triggerFilter();

        signals:
            //! Change the filter by enabling or disabling it
            void changeFilter(bool enable);

        protected:
            //! Set and connect filter bar buttonsd
            void setButtonsAndCount(CFilterBarButtons *buttons);

            //! Clear the filter form
            virtual void clearForm() = 0;

        private slots:
            //! Button clicked
            void ps_filterButtonClicked(CFilterBarButtons::FilterButton filterButton);

            //! Stylesheet changed
            void ps_onStyleSheetChanged();
        };

    } // namespace
} // namespace

#endif // guard
