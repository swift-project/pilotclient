#include "simulatorxplaneconfigwindow.h"
#include "ui_simulatorxplaneconfigwindow.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/fileutils.h"
#include <QStringBuilder>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStringBuilder>

using namespace BlackGui;

namespace
{
    QString xBusOriginDir()
    {
        return QCoreApplication::applicationDirPath() % QStringLiteral("/../xbus");
    }
}

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);

            ui->cp_XBusServer->addItem(BlackMisc::CDBusServer::sessionDBusServer());
            ui->cp_XBusServer->addItem(BlackMisc::CDBusServer::systemDBusServer());

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::ps_storeSettings);
            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::close);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);

            ui->cp_XBusServer->setCurrentText(m_xbusServerSetting.get());

            if (xBusAvailable())
                connect(ui->pb_InstallXBus, &QPushButton::clicked, this, &CSimulatorXPlaneConfigWindow::ps_installXBus);
            else
                ui->pb_InstallXBus->setEnabled(false);
        }

        CSimulatorXPlaneConfigWindow::~CSimulatorXPlaneConfigWindow()
        {

        }

        bool CSimulatorXPlaneConfigWindow::xBusAvailable()
        {
            return QDir(xBusOriginDir()).exists();
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
                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog);

            if (path.isEmpty()) // canceled
                return;

            path.append("/Resources/plugins");
            if (!QDir(path).exists())
            {
                QMessageBox::warning(this, tr("Invalid X-Plane directory"), tr("%1 is not a valid X-Plane installation.").arg(path));
                return;
            }

            path.append("/xbus");

            // TODO Use QtConcurrent here, maybe?
            bool result = BlackMisc::CFileUtils::copyRecursively(xBusOriginDir(), path);
            if (result)
            {
                QMessageBox::information(this, tr("XBus installed"), tr("You may now launch your X-Plane and start using XBus!"));
            }
            else
            {
                QMessageBox::warning(this, tr("Failed installing XBus"), tr("Failed installing the XBus plugin in your X-Plane installation directory; try installing it manually."));
            }
        }
    }
}
