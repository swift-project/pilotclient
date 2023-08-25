// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_FILTERS_FILTERWIDGET_H
#define BLACKGUI_FILTERS_FILTERWIDGET_H

#include "blackgui/blackguiexport.h"
#include "blackgui/filters/filterbarbuttons.h"

#include <QFrame>
#include <QObject>

class QWidget;

namespace BlackGui::Filters
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

        //! Row count has been changed
        virtual void onRowCountChanged(int count, bool withFilter) = 0;

        //! Trigger filtering
        virtual void triggerFilter();

    signals:
        //! Change the filter by enabling or disabling it
        void changeFilter(bool enable);

        //! A filter dialog is supposed to close
        void rejectDialog();

    protected:
        //! Set and connect filter bar buttons
        void setButtonsAndCount(CFilterBarButtons *buttons);

        //! Clear the filter form
        virtual void clearForm() = 0;

        //! Convert DB id
        static int convertDbId(const QString &candidate);

    private:
        //! Button clicked
        void onFilterButtonClicked(CFilterBarButtons::FilterButton filterButton);

        //! Stylesheet changed
        void onStyleSheetChanged();
    };
} // namespace

#endif // guard
