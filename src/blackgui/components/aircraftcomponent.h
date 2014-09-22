/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTCOMPONENT_H
#define BLACKGUI_AIRCRAFTCOMPONENT_H

#include "blackgui/components/runtimebasedcomponent.h"
#include "blackgui/components/timerbasedcomponent.h"
#include "blackgui/components/dockwidgetinfoareacomponent.h"
#include "blackmisc/avaircraft.h"
#include <QTabWidget>

namespace Ui { class CAircraftComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Aircraft widget
        class CAircraftComponent :
            public QTabWidget,
            public CDockWidgetInfoAreaComponent,
            public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CAircraftComponent();

            //! Timer for updating
            CTimerBasedComponent *getTimerComponent() { return this->m_timerComponent; }

            //! Aircrafts in range
            int countAircrafts() const;

            //! Airports in range
            int countAirportsInRange() const;

        public slots:
            //! Update aircrafts
            void update();

            //! \copydoc CTimerBasedComponent::setUpdateIntervalSeconds
            void setUpdateIntervalSeconds(int seconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateIntervalSeconds(seconds); }

            //! \copydoc CTimerBasedComponent::setUpdateInterval
            void setUpdateInterval(int milliSeconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateInterval(milliSeconds); }

            //! \copydoc CTimerBasedComponent::stopTimer
            void stopTimer() { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->stopTimer(); }

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private slots:
            //! Info area tab bar has changed
            void ps_infoAreaTabBarChanged(int index);

        private:
            Ui::CAircraftComponent *ui;
            CTimerBasedComponent   *m_timerComponent;
        };
    }
}

#endif // guard
