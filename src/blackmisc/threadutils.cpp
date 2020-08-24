/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/threadutils.h"

#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QtGlobal>
#include <QPointer>
#include <QTimer>
#include <thread>
#include <sstream>

namespace BlackMisc
{
    bool CThreadUtils::isInThisThread(const QObject *toBeTested)
    {
        return QThread::currentThread() == toBeTested->thread();
    }

    bool CThreadUtils::thisIsMainThread()
    {
        return qApp && QThread::currentThread() == qApp->thread();
    }

    QString CThreadUtils::currentThreadInfo()
    {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        const QThread *thread = QThread::currentThread();
        const QString id = QString::fromStdString(oss.str());
        return QStringLiteral("%1 (%2) prio %3").arg(id).arg(thread->objectName()).arg(thread->priority());
    }
} // ns
