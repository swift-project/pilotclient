/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "tool.h"
#include "blackcore/context_runtime.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>
#include <QMetaMethod>
#include <QApplication>
#include <QIcon>

/*!
 * DBus tests, tests marshalling / unmarshalling of many value classes.
 * Forks two processes and sends data via DBus among them.
 */
int main(int argc, char *argv[])
{
    // metadata are registered in runtime
    QApplication a(argc, argv); // not QCoreApplication because of icon, http://qt-project.org/forums/viewthread/15412
    QIcon icon(":/blackcore/icons/tower.png");
    QApplication::setWindowIcon(icon);
    BlackCore::CRuntime *core = new BlackCore::CRuntime(BlackCore::CRuntimeConfig::forCore(), &a);
    QtConcurrent::run(BlackMiscTest::Tool::serverLoop, core); // QFuture<void> future
    qDebug() << "Server event loop, pid:" << BlackMiscTest::Tool::getPid();

    // end
    return a.exec();
}
