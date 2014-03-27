#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackmisc/hwkeyboardkey.h"
#include "blackmisc/networkchecks.h"
#include "blacksim/fsx/simconnectutilities.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackSim::Fsx;

/*
 * Reload settings
 */
void MainWindow::reloadSettings()
{
    // local copy
    CSettingsNetwork nws = this->m_contextSettings->getNetworkSettings();

    // update servers
    this->m_modelTrafficServerList->setSelectedServer(nws.getCurrentTrafficNetworkServer());
    this->m_modelTrafficServerList->update(nws.getTrafficNetworkServers());
    this->ui->tv_SettingsTnServers->resizeColumnsToContents();
    this->ui->tv_SettingsTnServers->resizeRowsToContents();

    // update hot keys
    this->m_modelSettingsHotKeys->update(this->m_contextSettings->getHotkeys());
    this->ui->tv_SettingsMiscHotkeys->resizeColumnsToContents();
    this->ui->tv_SettingsMiscHotkeys->resizeRowsToContents();

    // fake setting for sound notifications
    this->ui->cb_SettingsAudioPlayNotificationSounds->setChecked(true);
    this->ui->cb_SettingsAudioNotificationTextMessage->setChecked(true);
}

/*
 * Network has been selected
 */
void MainWindow::networkServerSelected(QModelIndex index)
{
    const CServer clickedServer = this->m_modelTrafficServerList->at(index);
    this->updateGuiSelectedServerTextboxes(clickedServer);
}

/*
 * Alter server
 */
void MainWindow::alterTrafficServer()
{
    CServer server = this->selectedServerFromTextboxes();
    if (!server.isValidForLogin())
    {
        const CStatusMessage validation = CStatusMessage::getValidationError("Wrong settings for server");
        this->displayStatusMessage(validation);
        return;
    }

    const QString path = CSettingUtilities::appendPaths(IContextSettings::PathNetworkSettings(), CSettingsNetwork::ValueTrafficServers());
    QObject *sender = QObject::sender();
    CStatusMessageList msgs;
    if (sender == this->ui->pb_SettingsTnCurrentServer)
    {
        msgs = this->m_contextSettings->value(path, CSettingsNetwork::CmdSetCurrentServer(), server.toQVariant());
    }
    else if (sender == this->ui->pb_SettingsTnRemoveServer)
    {
        msgs = this->m_contextSettings->value(path, CSettingUtilities::CmdRemove(), server.toQVariant());
    }
    else if (sender == this->ui->pb_SettingsTnSaveServer)
    {
        msgs = this->m_contextSettings->value(path, CSettingUtilities::CmdUpdate(), server.toQVariant());
    }

    // status messages
    this->displayStatusMessages(msgs);
}

/*
 * Settings did changed
 */
void MainWindow::changedSettings()
{
    this->reloadSettings();
}

/*
 * Textboxes from server
 */
void MainWindow::updateGuiSelectedServerTextboxes(const CServer &server)
{
    this->ui->le_SettingsTnCsName->setText(server.getName());
    this->ui->le_SettingsTnCsDescription->setText(server.getDescription());
    this->ui->le_SettingsTnCsAddress->setText(server.getAddress());
    this->ui->le_SettingsTnCsPort->setText(QString::number(server.getPort()));
    this->ui->le_SettingsTnCsRealName->setText(server.getUser().getRealName());
    this->ui->le_SettingsTnCsNetworkId->setText(server.getUser().getId());
    this->ui->le_SettingsTnCsPassword->setText(server.getUser().getPassword());
}


/*
 * Server settings from textboxes
 */
CServer MainWindow::selectedServerFromTextboxes() const
{
    CServer server;
    bool portOk = false;
    server.setName(this->ui->le_SettingsTnCsName->text());
    server.setDescription(this->ui->le_SettingsTnCsDescription->text());
    server.setAddress(this->ui->le_SettingsTnCsAddress->text());
    server.setPort(this->ui->le_SettingsTnCsPort->text().toInt(&portOk));
    if (!portOk) server.setPort(-1);

    CUser user;
    user.setRealName(this->ui->le_SettingsTnCsRealName->text());
    user.setId(this->ui->le_SettingsTnCsNetworkId->text());
    user.setPassword(this->ui->le_SettingsTnCsPassword->text());
    server.setUser(user);

    return server;
}

/*
 * Save the hotkeys
 */
void MainWindow::saveHotkeys()
{
    const QString path = CSettingUtilities::appendPaths(IContextSettings::PathRoot(), IContextSettings::PathHotkeys());
    CStatusMessageList msgs = this->m_contextSettings->value(path, CSettingUtilities::CmdUpdate(), this->m_modelSettingsHotKeys->getContainer().toQVariant());

    // status messages
    this->displayStatusMessages(msgs);
}

void MainWindow::clearHotkey()
{
    QModelIndex i = this->ui->tv_SettingsMiscHotkeys->currentIndex();
    if (i.row() < 0 || i.row() >= this->m_modelSettingsHotKeys->rowCount()) return;
    BlackMisc::Hardware::CKeyboardKey key = this->m_modelSettingsHotKeys->at(i);
    BlackMisc::Hardware::CKeyboardKey defKey;
    defKey.setFunction(key.getFunction());
    this->m_modelSettingsHotKeys->update(i, defKey);
}

void MainWindow::testSimConnectConnection()
{
    QString address = this->ui->le_SettingsSimulatorFsxAddress->text().trimmed();
    QString port = this->ui->le_SettingsSimulatorFsxPort->text().trimmed();

    if (address.isEmpty() || port.isEmpty())
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "no address or port"));
        return;
    }
    if (!CNetworkChecks::isValidIPv4Address(address))
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "IPv4 address invalid"));
        return;
    }
    if (!CNetworkChecks::isValidPort(port))
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "invalid port"));
        return;
    }
    quint16 p = port.toUInt();
    QString msg;
    if (!CNetworkChecks::canConnect(address, p, msg))
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, msg));
        return;
    }

    msg = QString("Connected to %1:%2").arg(address).arg(port);
    this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, msg));
}

void MainWindow::saveSimConnectCfg()
{
    QString address = this->ui->le_SettingsSimulatorFsxAddress->text().trimmed();
    QString port = this->ui->le_SettingsSimulatorFsxPort->text().trimmed();

    if (address.isEmpty() || port.isEmpty())
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "no address or port"));
        return;
    }
    if (!CNetworkChecks::isValidIPv4Address(address))
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "IPv4 address invalid"));
        return;
    }
    if (!CNetworkChecks::isValidPort(port))
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "invalid port"));
        return;
    }
    quint16 p = port.toUInt();
    QString file = CSimConnectUtilities::getLocalSimConnectCfgFilename();
    if (CSimConnectUtilities::writeSimConnectCfg(file, address, p))
    {
        QString m = QString("Written ").append(file);
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, m));
    }
    else
    {
        QString m = QString("Cannot write ").append(file);
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, m));
    }
}
