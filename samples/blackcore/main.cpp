/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_runtime.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackcore/context_settings.h"
#include "tool.h"
#include <QtConcurrent/QtConcurrent>

#include <QMetaType>
#include <QMetaMethod>


/*!
 * DBus tests, tests marshalling / unmarshalling of many value classes.
 * Forks two processes and sends data via DBus among them.
 */
int main(int argc, char *argv[])
{
    // metadata are registered in runtime
    QCoreApplication a(argc, argv);
    BlackCore::CRuntime *core = new BlackCore::CRuntime(BlackCore::CRuntimeConfig::forCore(), &a);
    QtConcurrent::run(BlackMiscTest::Tool::serverLoop, core); // QFuture<void> future
    qDebug() << "Server event loop, pid:" << BlackMiscTest::Tool::getPid();

    // end
    return a.exec();
}
