// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/threadutils.h"

#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QtGlobal>
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
