//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef BLACKD_H
#define BLACKD_H

#include <QSystemTrayIcon>
#include <QDialog>
#include "blackmisc/com_server.h"
#include "blackmisc/gui_messages.h"

#include "blackmisc/message_system.h"

namespace Ui {
class BlackD;
}

namespace FSD {
class CFSDClient;
}

class CQtDisplayer;
class CMultiPlayer;

class BlackD : public QDialog, public BlackMisc::CMessageHandler
{
    Q_OBJECT
    
public:
    explicit BlackD(QWidget *parent = 0);
    ~BlackD();

    void setVisible(bool visible);

protected:
     void closeEvent(QCloseEvent *event);

private slots:
     void iconActivated(QSystemTrayIcon::ActivationReason reason);
	 void onData(QString &messageID, QByteArray &message);
    
private:

    void createActions();
    void createTrayIcon();
    void createLogging();
	void createComServer();

	//! Messages
	void onMSG_CONNECT_TO_VATSIM(const BlackMisc::MSG_CONNECT_TO_VATSIM *connect);

    Ui::BlackD *ui;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    CQtDisplayer *m_displayer;

    FSD::CFSDClient      *m_fsd_client;
    CMultiPlayer	*m_multi_player;

	BlackMisc::CComServer *m_comserver;

};

#endif // BLACKD_H
