//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/context.h"

#include "blackd.h"
#include <QApplication>
#include <QtGui>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(blackd);

    QApplication app(argc, argv);
    BlackMisc::CApplicationContext myBlackApp;

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
             QMessageBox::critical(0, QObject::tr("Systray"),
                                   QObject::tr("I couldn't detect any system tray "
                                               "on this system."));
             return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    BlackD w;
    w.hide();
    
    return app.exec();
}
