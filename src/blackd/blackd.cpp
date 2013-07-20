//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <QtGui>
#include <QMessageBox>
#include <QMenu>

#include "blackmisc/context.h"
#include "blackmisc/debug.h"
#include "blackmisc/message_factory.h"
#include "blackcore/fsd_client.h"

#include "qt_displayer.h"

#include "blackd.h"
#include "ui_blackd.h"

using namespace FSD;


BlackD::BlackD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlackD)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    connect(trayIcon, &QSystemTrayIcon::activated,
            this,     &BlackD::iconActivated);

    setWindowTitle(tr("BlackD"));

    QIcon icon = QIcon(":/images/blackbox_icon.svg");
    trayIcon->setIcon(icon);
    trayIcon->show();

    setWindowIcon(icon);

    createLogging();

    createComServer();

    m_fsd_client = new CFSDClient(BlackMisc::IContext::getInstance());

    bAppDebug << "BlackDaemon running...";
}

BlackD::~BlackD()
{
    delete ui;
}

void BlackD::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void BlackD::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible())
    {
        QMessageBox::information(this, tr("BlackD"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}

void BlackD::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        setVisible(!isVisible());
        break;
    default:
        break;
    }
}

void BlackD::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void BlackD::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void BlackD::createLogging()
{
    BlackMisc::IContext::getInstance().getDebug()->create();

    m_displayer = new CQtDisplayer(ui->logginView);

    BlackMisc::IContext::getInstance().getDebug()->getDebugLog()->attachDisplay(m_displayer);
    BlackMisc::IContext::getInstance().getDebug()->getInfoLog()->attachDisplay(m_displayer);
    BlackMisc::IContext::getInstance().getDebug()->getWarningLog()->attachDisplay(m_displayer);
    BlackMisc::IContext::getInstance().getDebug()->getErrorLog()->attachDisplay(m_displayer);
}

void BlackD::createComServer()
{
    BlackMisc::CMessageFactory::getInstance().registerMessages();
    m_comserver = new BlackMisc::CComServer(BlackMisc::IContext::getInstance(), this);

    registerMessageFunction(this, &BlackD::onMSG_CONNECT_TO_VATSIM);

    QHostAddress local = QHostAddress(QHostAddress::LocalHost);

    m_comserver->Host(local, 42000);
    connect(m_comserver, SIGNAL(doMessageReceived(QString &, QByteArray &)), this, SLOT(onData(QString &, QByteArray &)));
}

void BlackD::onData(QString &messageID, QByteArray &message)
{
    bAppDebug << messageID;
    BlackMisc::IMessage *test = BlackMisc::CMessageFactory::getInstance().create(messageID);
    QDataStream stream(&message, QIODevice::ReadOnly);

    Q_ASSERT(test);
    *test << stream;

    BlackMisc::CMessageDispatcher::getInstance().append(test);
    BlackMisc::CMessageDispatcher::getInstance().dispatch();
}

void BlackD::onMSG_CONNECT_TO_VATSIM(const BlackMisc::MSG_CONNECT_TO_VATSIM *connect)
{
    bAppDebug << "Connecting to FSD server:";
    bAppDebug << connect->getHost() << ":" << connect->getPort();

    FSD::TClientInfo clientinfo;
    clientinfo.m_callsign = connect->getCallsign();
    clientinfo.m_host = connect->getHost();
    clientinfo.m_password = connect->getPassword();
    clientinfo.m_port = connect->getPort();
    clientinfo.m_realName = connect->getRealName();
    clientinfo.m_simType = FSD::SIM_UNKNOWN;
    clientinfo.m_userid = connect->getUserID();

    m_fsd_client->updateClientInfo(clientinfo);

    m_fsd_client->connectTo(connect->getHost(), connect->getPort());
}
