/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "updatetimer.h"

namespace BlackGui
{
    namespace Components
    {
        CUpdateTimer::CUpdateTimer(const char *slot, QObject *parent) :
            QObject(parent)
        {
            Q_ASSERT(parent);
            this->m_timer = new QTimer(this);
            this->m_timerSingleShot = new QTimer(this);
            this->m_timerSingleShot->setSingleShot(true);
            this->connect(this->m_timer, SIGNAL(timeout()), parent, slot);
            this->connect(this->m_timerSingleShot, SIGNAL(timeout()), parent, slot);
        }

        CUpdateTimer::~CUpdateTimer()
        {
            this->m_timer->stop();
            this->m_timerSingleShot->stop();
            if (this->parent()) this->disconnect(this->parent());
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
    }
} // namespace
