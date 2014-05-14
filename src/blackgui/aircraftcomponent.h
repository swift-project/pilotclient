#ifndef BLACKGUI_AIRCRAFTCOMPONENT_H
#define BLACKGUI_AIRCRAFTCOMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include "blackgui/timerbasedcomponent.h"
#include "blackmisc/avaircraft.h"

#include <QTabWidget>

namespace Ui { class CAircraftComponent; }
namespace BlackGui
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

#endif // guard
