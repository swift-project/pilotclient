#include "flightplanwidget.h"
#include "ui_flightplanwidget.h"

namespace BlackGui
{
    CFlightplanWidget::CFlightplanWidget(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CFlightplanWidget)
    {
        ui->setupUi(this);
    }

    CFlightplanWidget::~CFlightplanWidget()
    {
        delete ui;
    }
}
