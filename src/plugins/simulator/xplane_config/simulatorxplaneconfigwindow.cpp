#include "simulatorxplaneconfigwindow.h"
#include "ui_simulatorxplaneconfigwindow.h"
#include "blackcore/dbus_server.h"

using namespace BlackGui;

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow() :
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);

            ui->cp_XBusServer->addItem(BlackCore::CDBusServer::sessionDBusServer());
            ui->cp_XBusServer->addItem(BlackCore::CDBusServer::systemDBusServer());

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::ps_storeSettings);
            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::close);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);

            ui->cp_XBusServer->setCurrentText(m_xbusServerSetting.get());
        }

        CSimulatorXPlaneConfigWindow::~CSimulatorXPlaneConfigWindow()
        {

        }

        void CSimulatorXPlaneConfigWindow::ps_storeSettings()
        {
            if (ui->cp_XBusServer->currentText() != m_xbusServerSetting.get())
            {
                m_xbusServerSetting.set(ui->cp_XBusServer->currentText());
            }
        }

    }
}
