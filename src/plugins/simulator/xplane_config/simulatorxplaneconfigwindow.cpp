#include "simulatorxplaneconfigwindow.h"
#include "ui_simulatorxplaneconfigwindow.h"
#include "blackcore/dbus_server.h"


namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow() :
            QWidget(nullptr),
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);

            ui->cp_XBusServer->addItem(BlackCore::CDBusServer::sessionDBusServer());
            ui->cp_XBusServer->addItem(BlackCore::CDBusServer::systemDBusServer());

            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);
        }

        CSimulatorXPlaneConfigWindow::~CSimulatorXPlaneConfigWindow()
        {

        }

    }
}
