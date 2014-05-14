#ifndef BLACKGUI_TIMERBASEDCOMPONENT_H
#define BLACKGUI_TIMERBASEDCOMPONENT_H

#include <QTimer>

namespace BlackGui
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

#endif // guard
