/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/updatetimer.h"

namespace BlackGui
{
    namespace Components
    {
        void CUpdateTimer::initTimers(const QString &name)
        {
            this->m_timer = new QTimer(this);
            this->m_timerSingleShot = new QTimer(this);
            this->m_timerSingleShot->setSingleShot(true);
            this->m_timer->setObjectName(name + ":periodically");
            this->m_timerSingleShot->setObjectName(name + ":singleShot");
        }

        CUpdateTimer::~CUpdateTimer()
        {
            if (this->parent()) { this->disconnect(this->parent()); }
            this->m_timer->stop();
            this->m_timerSingleShot->stop();
        }

        void CUpdateTimer::setUpdateInterval(int milliSeconds)
        {
            if (milliSeconds < 100)
            {
                this->m_timer->stop();
            }
            else
            {
                this->m_timer->setInterval(milliSeconds);
                if (!this->m_timer->isActive()) this->m_timer->start();
            }
        }

        void CUpdateTimer::fireTimer()
        {
            Q_ASSERT(this->m_timerSingleShot);
            this->m_timer->start(); // restart other timer
            this->m_timerSingleShot->start(10);
        }
    } // namespace
} // namespace
