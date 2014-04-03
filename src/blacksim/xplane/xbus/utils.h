/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_XBUS_UTILS_H
#define BLACKSIM_XBUS_UTILS_H

#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMUtilities.h>
#include <QApplication>
#include <QSharedPointer>

/*!
 * \file
 */

/*!
 * QApplication subclass used by XBus.
 *
 * Only one instance of QApplication is allowed per process, so if any other
 * X-Plane plugin, completely unrelated to this pilot client, wants to use the
 * Qt framework, they can simply copy & paste this class into their project
 * and both X-Plane plugins will be able to share a single QApplication safely.
 */
class QSharedApplication : public QApplication
{
    Q_OBJECT

    QWeakPointer<QApplication> m_weakptr;

    QSharedApplication(QSharedPointer<QApplication> &ptr, int argc, char **argv) : QApplication(argc, argv)
    {
        ptr.reset(this);
        m_weakptr = ptr;
    }

public:
    static QSharedPointer<QApplication> sharedInstance()
    {
        QSharedPointer<QApplication> ptr;
        if (! instance())
        {
            static int argc = 1;
            static char *argv[] = { "X-Plane" };
            new QSharedApplication(ptr, argc, argv);
        }
        if (! instance()->inherits("QSharedApplication"))
        {
            XPLMDebugString("Error: there is an unshared QApplication in another plugin\n");
        }
        return static_cast<QSharedApplication*>(instance())->m_weakptr;
    }
};

/*!
 * Runs the Qt event loop inside the X-Plane event loop.
 */
class QXPlaneEventLoop : public QObject
{
    Q_OBJECT

    QXPlaneEventLoop(QObject *parent) : QObject(parent)
    {
        XPLMRegisterFlightLoopCallback(callback, -1, nullptr);
    }

    ~QXPlaneEventLoop()
    {
        XPLMUnregisterFlightLoopCallback(callback, nullptr);
    }

    static float callback(float, float, int, void *)
    {
        QApplication::processEvents();
        QApplication::sendPostedEvents();
        return -1;
    }

public:
    static void exec()
    {
        if (! QApplication::instance()->findChild<QXPlaneEventLoop *>())
        {
            new QXPlaneEventLoop(QApplication::instance());
        }
    }
};

#endif // guard
