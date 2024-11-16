// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_FILTERS_FILTERBARBUTTONS_H
#define SWIFT_GUI_FILTERS_FILTERBARBUTTONS_H

#include <QFrame>
#include <QMetaType>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

class QWidget;

namespace Ui
{
    class CFilterBarButtons;
}
namespace swift::gui::filters
{
    /*!
     * Filter bar buttons
     */
    class SWIFT_GUI_EXPORT CFilterBarButtons : public QFrame
    {
        Q_OBJECT

    public:
        //! Buttons
        enum FilterButton
        {
            ClearForm,
            Filter,
            RemoveFilter
        };

        //! Constructor
        explicit CFilterBarButtons(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFilterBarButtons();

        //! Show the counter
        void displayCount(bool show);

    signals:
        //! Filter button clicked
        void buttonClicked(swift::gui::filters::CFilterBarButtons::FilterButton filterButton);

    public slots:
        //! Row count has been changed
        void onRowCountChanged(int count, bool withFilter);

        //! Trigger button
        void clickButton(swift::gui::filters::CFilterBarButtons::FilterButton filterButton);

    private slots:
        //! Button was clicked
        void ps_buttonClicked();

    private:
        QScopedPointer<Ui::CFilterBarButtons> ui;
    };
} // namespace swift::gui::filters

Q_DECLARE_METATYPE(swift::gui::filters::CFilterBarButtons::FilterButton)

#endif // guard
