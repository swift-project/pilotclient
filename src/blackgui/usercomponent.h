#ifndef BLACKGUI_USERCOMPONENT_H
#define BLACKGUI_USERCOMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include "blackmisc/nwuserlist.h"

#include <QTabWidget>
#include <QTimer>

namespace Ui { class CUserComponent; }

namespace BlackGui
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

    public slots:
        //! Update users
        void update();

        //! Update time, time < 100 stops updates
        void setUpdateInterval(int milliSeconds);

        //! Update time
        void setUpdateIntervalSeconds(int seconds) { this->setUpdateInterval(1000 * seconds); }

    private:
        Ui::CUserComponent *ui;
        QTimer *m_timer;
    };
}

#endif // guard
