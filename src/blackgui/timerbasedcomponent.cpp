#include "timerbasedcomponent.h"

namespace BlackGui
{

    CTimerBasedComponent::CTimerBasedComponent(const char *slot, QObject *parent) :
        QObject(parent), m_timer(nullptr)
    {
        this->m_timer = new QTimer(this);
        this->connect(this->m_timer, SIGNAL(timeout()), parent, slot);
    }

    CTimerBasedComponent::~CTimerBasedComponent()
    {
        this->m_timer->stop();
        this->disconnect(this->parent());
    }

    void CTimerBasedComponent::setUpdateInterval(int milliSeconds)
    {
        if (milliSeconds < 100)
            this->m_timer->stop();
        else
        {
            this->m_timer->setInterval(milliSeconds);
            if (!this->m_timer->isActive()) this->m_timer->start();
        }
    }
} // guard
