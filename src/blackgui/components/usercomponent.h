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

#include "blackgui/components/runtimebasedcomponent.h"
#include "blackgui/components/timerbasedcomponent.h"
#include "blackmisc/nwuserlist.h"

#include <QTabWidget>
#include <QTimer>

namespace Ui { class CUserComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! User componenet (users, clients)
        class CUserComponent : public QTabWidget, public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CUserComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CUserComponent();

            //! Timer for updating
            CTimerBasedComponent *getTimerComponent() { return this->m_timerComponent; }

        public slots:
            //! Update users
            void update();

            //! \copydoc CTimerBasedComponent::setUpdateIntervalSeconds
            void setUpdateIntervalSeconds(int seconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateIntervalSeconds(seconds); }

            //! \copydoc CTimerBasedComponent::setUpdateInterval
            void setUpdateInterval(int milliSeconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateInterval(milliSeconds); }

            //! \copydoc CTimerBasedComponent::stopTimer
            void stopTimer() { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->stopTimer(); }

        private:
            Ui::CUserComponent *ui;
            CTimerBasedComponent *m_timerComponent;
        };
    }
}
#endif // guard
