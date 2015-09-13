#include "simulatorxplaneconfigwindow.h"
#include "ui_simulatorxplaneconfigwindow.h"
#include "blackcore/dbus_server.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include <QStringBuilder>
#include <QFileDialog>
#include <QMessageBox>

using namespace BlackGui;

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);

            ui->cp_XBusServer->addItem(BlackCore::CDBusServer::sessionDBusServer());
            ui->cp_XBusServer->addItem(BlackCore::CDBusServer::systemDBusServer());

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::ps_storeSettings);
            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::close);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);

            ui->cp_XBusServer->setCurrentText(m_xbusServerSetting.get());

            connect(ui->pb_InstallXBus, &QPushButton::clicked, this, &CSimulatorXPlaneConfigWindow::ps_installXBus);
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

        void CSimulatorXPlaneConfigWindow::ps_installXBus()
        {
            QString xPlaneLocation = BlackMisc::Simulation::XPlane::CXPlaneUtil::xplane10Dir();
            if (xPlaneLocation.isEmpty())
                xPlaneLocation = BlackMisc::Simulation::XPlane::CXPlaneUtil::xplane9Dir();

            QString path = QFileDialog::getExistingDirectory(parentWidget(),
                           tr("Choose your X-Plane install directory"),
                           xPlaneLocation,
                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            path.append("/Resources/plugins/xbus/64/");
            QDir xbusDir(path);
            if (!xbusDir.exists())
            {
                if (!xbusDir.mkpath("."))
                {
                    return;
                }
            }

            QString origin = QCoreApplication::applicationDirPath() % QStringLiteral("/../xbus/64/lin.xpl");
            QString destination = path % "/lin.xpl";
            QFile::copy(origin, destination);

            QMessageBox::information(this, tr("XBus installed"), tr("You may now launch your X-Plane and start using XBus!"));
        }
    }
}
