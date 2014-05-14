#include "aircraftcomponent.h"
#include "ui_aircraftcomponent.h"

namespace BlackGui
{

    CAircraftComponent::CAircraftComponent(QWidget *parent) :
        QTabWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CAircraftComponent), m_timerComponent(nullptr)
    {
        ui->setupUi(this);
        m_timerComponent = new CTimerBasedComponent(SLOT(update()), this);
    }

    CAircraftComponent::~CAircraftComponent()
    {
        delete ui;
    }

    void CAircraftComponent::update()
    {
        Q_ASSERT(this->ui->tvp_AircraftsInRange);
        Q_ASSERT(this->getIContextNetwork());

        if (this->getIContextNetwork()->isConnected())
        {
            this->ui->tvp_AircraftsInRange->update(this->getIContextNetwork()->getAircraftsInRange());
        }
    }
}
