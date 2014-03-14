/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/coreruntime.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "tool.h"
#include <QtConcurrent/QtConcurrent>


/*!
 * DBus tests, tests marshalling / unmarshalling of many value classes.
 * Forks two processes and sends data via DBus among them.
 */
int main(int argc, char *argv[])
{
    // metadata are registered in runtime
    QCoreApplication a(argc, argv);
    BlackCore::CCoreRuntime *core = new BlackCore::CCoreRuntime(true, &a);

    QtConcurrent::run(BlackMiscTest::Tool::serverLoop, core); // QFuture<void> future
    qDebug() << "Server event loop, pid:" << BlackMiscTest::Tool::getPid();

    // loop
    return a.exec();
}
