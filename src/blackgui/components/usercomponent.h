/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_USERCOMPONENT_H
#define BLACKGUI_USERCOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/components/updatetimer.h"

#include <QTabWidget>
#include <QTimer>
#include <QScopedPointer>

namespace Ui { class CUserComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! User componenet (users, clients)
        class CUserComponent :
            public QTabWidget,
            public CEnableForDockWidgetInfoArea,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CUserComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CUserComponent();

            //! Timer for updating
            CUpdateTimer *getTimerComponent() { return this->m_updateTimer; }

            //! Number of clients
            int countClients() const;

            //! Number of users
            int countUsers() const;

        public slots:
            //! Update users
            void update();

            //! \copydoc CTimerBasedComponent::setUpdateIntervalSeconds
            void setUpdateIntervalSeconds(int seconds) { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->setUpdateIntervalSeconds(seconds); }

            //! \copydoc CTimerBasedComponent::setUpdateInterval
            void setUpdateInterval(int milliSeconds) { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->setUpdateInterval(milliSeconds); }

            //! \copydoc CTimerBasedComponent::stopTimer
            void stopTimer() { Q_ASSERT(this->m_updateTimer); this->m_updateTimer->stopTimer(); }

        private slots:
            //! Number of elements changed
            void ps_countChanged(int count);

        private:
            QScopedPointer<Ui::CUserComponent> ui;
            CUpdateTimer *m_updateTimer;
        };
    }
}
#endif // guard
