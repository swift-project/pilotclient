/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TIMERBASEDCOMPONENT_H
#define BLACKGUI_TIMERBASEDCOMPONENT_H

#include <QTimer>

namespace BlackGui
{
    namespace Components
    {
        //! Timer based componenet
        class CTimerBasedComponent: public QObject
        {
        public:
            //! Constructor
            CTimerBasedComponent(const char *slot, QObject *parent);

            //! Destructor
            ~CTimerBasedComponent();

        public slots:
            //! Update time, time < 100 stops updates
            void setUpdateInterval(int milliSeconds);

            //! Update time
            void setUpdateIntervalSeconds(int seconds) { this->setUpdateInterval(1000 * seconds); }

            //! Stop timer
            void stopTimer() { this->setUpdateInterval(-1); }

        private:
            QTimer *m_timer;
        };
    }
}
#endif // guard
