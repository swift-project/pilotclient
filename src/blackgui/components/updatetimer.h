/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_UPDATETIMER_H
#define BLACKGUI_UPDATETIMER_H

#include "blackgui/blackguiexport.h"

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QtGlobal>

namespace BlackGui
{
    namespace Components
    {
        //! Timer used in components for updates
        //! \deprecated used at the beginning of the project, likely to be removed in the future
        class BLACKGUI_EXPORT CUpdateTimer: public QObject
        {
            Q_OBJECT

        public:
            //! Construct a timer which forwards messages to the given slot of parent.
            template <typename F, typename P>
            CUpdateTimer(const QString &name, F slot, P *parent) : QObject(parent)
            {
                Q_ASSERT(parent);
                this->initTimers(name);
                bool c = this->connect(this->m_timer, &QTimer::timeout, parent, slot);
                Q_ASSERT(c);
                c = this->connect(this->m_timerSingleShot, &QTimer::timeout, parent, slot);
                Q_ASSERT(c);
                Q_UNUSED(c);
            }

            //! Destructor
            virtual ~CUpdateTimer();

            //! Date/time of 1/1/1970, used to init timestamp values as "outdated"
            static const QDateTime &epoch()
            {
                static const QDateTime e = QDateTime::fromMSecsSinceEpoch(0);
                return e;
            }

        public slots:
            //! Update time, time < 100ms stops updates
            void setUpdateInterval(int milliSeconds);

            //! Update time
            void setUpdateIntervalSeconds(int seconds) { this->setUpdateInterval(1000 * seconds); }

            //! Stop timer
            void stopTimer() { this->setUpdateInterval(-1); this->m_timerSingleShot->stop(); }

            //! Start timer
            void startTimer(int milliSeconds) { this->setUpdateInterval(milliSeconds);}

            //! Fire the timer straight away
            void fireTimer();

        private:
            void initTimers(const QString &name); //!< init timers
            QTimer *m_timer = nullptr;            //!< periodically updating
            QTimer *m_timerSingleShot = nullptr;  //!< single update
        };
    } // ns
} // ns
#endif // guard
