// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_FILTERWIDGET_H
#define SWIFT_GUI_FILTERS_FILTERWIDGET_H

#include <QFrame>
#include <QObject>

#include "gui/filters/filterbarbuttons.h"
#include "gui/swiftguiexport.h"

class QWidget;

namespace swift::gui::filters
{
    //! Base for filter dialog
    class SWIFT_GUI_EXPORT CFilterWidget : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        CFilterWidget(QWidget *parent = nullptr);

        //! Destructor
        ~CFilterWidget() override;

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
} // namespace swift::gui::filters

#endif // SWIFT_GUI_FILTERS_FILTERWIDGET_H
