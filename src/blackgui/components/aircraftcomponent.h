/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_AIRCRAFTCOMPONENT_H
#define BLACKGUI_AIRCRAFTCOMPONENT_H

//! \file

#include "blackgui/components/runtimebasedcomponent.h"
#include "blackgui/components/timerbasedcomponent.h"
#include "blackmisc/avaircraft.h"

#include <QTabWidget>

namespace Ui { class CAircraftComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Aircraft widget
        class CAircraftComponent : public QTabWidget, public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CAircraftComponent();

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
            Ui::CAircraftComponent *ui;
            CTimerBasedComponent *m_timerComponent;
        };
    }
}

#endif // guard
