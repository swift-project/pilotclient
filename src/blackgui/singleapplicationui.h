// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SINGLEAPPLICATIONUI_H
#define BLACKGUI_SINGLEAPPLICATIONUI_H

#include <QTimer>

class QWidget;

namespace BlackGui
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
} // ns

#endif // guard
