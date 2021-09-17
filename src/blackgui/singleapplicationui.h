/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
