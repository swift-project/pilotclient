#include "usercomponent.h"
#include "ui_usercomponent.h"

namespace BlackGui
{
    CUserComponent::CUserComponent(QWidget *parent) :
        QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CUserComponent), m_timerComponent(nullptr)
    {
        ui->setupUi(this);
        this->m_timerComponent = new CTimerBasedComponent(SLOT(update()), this);
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
} // guard
