/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "introwindow.h"
#include "ui_introwindow.h"
#include "blackcore/dbus_server.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/project.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QProcess>
#include <QFileInfo>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackGui;

/*
 * Constructor
 */
CIntroWindow::CIntroWindow(QWidget *parent) :
    QDialog(parent, (Qt::WindowStaysOnTopHint)),
    // (Qt::Tool | Qt::WindowStaysOnTopHint)),
    ui(new Ui::CIntroWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(CProject::swiftVersionStringDevInfo());
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->ui->cb_DBusServer->addItem(CDBusServer::sessionDBusServer());
    this->ui->cb_DBusServer->addItem(CDBusServer::systemDBusServer());
    this->ui->cb_DBusServer->addItems(CNetworkUtils::getKnownIpAddresses());
    this->ui->cb_DBusServer->setCurrentIndex(0);
}

/*
 * Destructor
 */
CIntroWindow::~CIntroWindow() { }

/*
 * Window mode
 */
BlackGui::CEnableForFramelessWindow::WindowMode CIntroWindow::getWindowMode() const
{
    if (this->ui->rb_WindowFrameless->isChecked())
        return CEnableForFramelessWindow::WindowFrameless;
    else
        return CEnableForFramelessWindow::WindowTool;
}

/*
 * Core mode
 */
GuiModes::CoreMode CIntroWindow::getCoreMode() const
{
    if (this->ui->rb_CoreExternalVoiceLocal->isChecked())
    {
        return GuiModes::CoreExternalAudioLocal;
    }
    else if (this->ui->rb_CoreInGuiProcess->isChecked())
    {
        return GuiModes::CoreInGuiProcess;
    }
    else
    {
        return GuiModes::CoreExternal;
    }
}

/*
 * DBus server address
 */
QString CIntroWindow::getDBusAddress() const
{
    return this->ui->cb_DBusServer->currentText();
}

/*
 * Button clicked
 */
void CIntroWindow::buttonClicked() const
{
    QObject *sender = QObject::sender();
    if (sender == this->ui->pb_ModelDb)
    {
        QDesktopServices::openUrl(QUrl("http://vatrep.vatsim-germany.org/page/index.php", QUrl::TolerantMode));
    }
    else if (sender == this->ui->pb_WebSite)
    {
        QDesktopServices::openUrl(QUrl("https://dev.vatsim-germany.org/", QUrl::TolerantMode));
    }
    else if (sender == this->ui->pb_SettingsDir)
    {
        QString path = QDir::toNativeSeparators(BlackMisc::Settings::CSettingUtilities::getSettingsDirectory());
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
    else if (sender == this->ui->pb_CoreStart)
    {
        //! \todo make fully OS independent
        QString sfx = QFileInfo(QCoreApplication::applicationFilePath()).suffix();
        QString core = QDir(QApplication::applicationDirPath()).filePath("swiftcore." + sfx);
        QProcess::startDetached(core);
    }
}
