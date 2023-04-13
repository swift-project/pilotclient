/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_FILTERBARBUTTONS_H
#define BLACKGUI_FILTERS_FILTERBARBUTTONS_H

#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QMetaType>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui
{
    class CFilterBarButtons;
}
namespace BlackGui::Filters
{
    /*!
     * Filter bar buttons
     */
    class BLACKGUI_EXPORT CFilterBarButtons : public QFrame
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
        void buttonClicked(BlackGui::Filters::CFilterBarButtons::FilterButton filterButton);

    public slots:
        //! Row count has been changed
        void onRowCountChanged(int count, bool withFilter);

        //! Trigger button
        void clickButton(BlackGui::Filters::CFilterBarButtons::FilterButton filterButton);

    private slots:
        //! Button was clicked
        void ps_buttonClicked();

    private:
        QScopedPointer<Ui::CFilterBarButtons> ui;
    };
} // ns

Q_DECLARE_METATYPE(BlackGui::Filters::CFilterBarButtons::FilterButton)

#endif // guard
