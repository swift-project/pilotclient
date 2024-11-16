// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SINGLEAPPLICATIONUI_H
#define SWIFT_GUI_SINGLEAPPLICATIONUI_H

#include <QTimer>

class QWidget;

namespace swift::gui
{
    /*!
     * Enable / disable UI depending on how many swift applications are running
     */
    class CSingleApplicationUi
    {
    public:
        //! Constructor
        CSingleApplicationUi(QWidget *ui);

        //! Set UI widget
        void setWidget(QWidget *ui) { m_ui = ui; }

    private:
        //! Evaluate if
        void evaluate();

        QTimer m_timer;
        QWidget *m_ui = nullptr; //!< enabled/disabled UI
    };
} // namespace swift::gui

#endif // guard
