#include "usercomponent.h"
#include "ui_usercomponent.h"

namespace BlackGui
{

    CUserComponent::CUserComponent(QWidget *parent) :
        QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CUserComponent), m_timer(nullptr)
    {
        ui->setupUi(this);
        this->m_timer = new QTimer(this);
        this->connect(this->m_timer, &QTimer::timeout, this, &CUserComponent::update);
    }

    CUserComponent::~CUserComponent()
    {
        delete ui;
    }

    void CUserComponent::update()
    {
        Q_ASSERT(this->ui->tvp_AllUsers);
        Q_ASSERT(this->ui->tvp_Clients);
        Q_ASSERT(this->getIContextNetwork());

        if (this->getIContextNetwork()->isConnected())
        {
            this->ui->tvp_Clients->update(this->getIContextNetwork()->getOtherClients());
            this->ui->tvp_AllUsers->update(this->getIContextNetwork()->getUsers());
        }
    }

    void CUserComponent::setUpdateInterval(int milliSeconds)
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
