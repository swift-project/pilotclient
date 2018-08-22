/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/threadutils.h"

#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QtGlobal>

namespace BlackMisc
{
    bool CThreadUtils::isCurrentThreadObjectThread(const QObject *toBeTested)
    {
        Q_ASSERT_X(toBeTested, Q_FUNC_INFO, "missing QObject");
        Q_ASSERT_X(toBeTested->thread(), Q_FUNC_INFO, "missing thread");
        return (QThread::currentThread() == toBeTested->thread());
    }

    bool CThreadUtils::isApplicationThreadObjectThread(const QObject *toBeTested)
    {
        Q_ASSERT_X(toBeTested, Q_FUNC_INFO, "missing QObject");
        Q_ASSERT_X(toBeTested->thread(), Q_FUNC_INFO, "missing thread");
        if (!QCoreApplication::instance() || !QCoreApplication::instance()->thread()) { return false; }
        return (QCoreApplication::instance()->thread() == toBeTested->thread());
    }

    bool CThreadUtils::isCurrentThreadApplicationThread()
    {
        if (!QCoreApplication::instance()) { return false; }
        if (!QCoreApplication::instance()->thread()) { return false; }
        return (QCoreApplication::instance()->thread() == QThread::currentThread());
    }

    const QString &CThreadUtils::priorityToString(QThread::Priority priority)
    {
        static const QString idle("idle");
        static const QString lowest("lowest");
        static const QString low("low");
        static const QString normal("normal");
        static const QString high("high");
        static const QString highest("highest");

        switch (priority)
        {
        case QThread::IdlePriority: return idle;
        case QThread::LowestPriority: return lowest;
        case QThread::LowPriority: return low;
        case QThread::NormalPriority: return normal;
        case QThread::HighPriority: return high;
        case QThread::HighestPriority: return highest;
        default: break;
        }

        static const QString unknown("unknown");
        return unknown;
    }

    const QString CThreadUtils::threadToString(const void *t)
    {
        static const QString s("0x%1");
        return s.arg(reinterpret_cast<long long>(t), 0, 16);
    }

    const QString CThreadUtils::currentThreadInfo()
    {
        static const QString info("thread: %1 name: '%2' priority: '%3'");
        const QThread *t = QThread::currentThread();
        if (!t) { return QString("no thread"); }
        return info.arg(threadToString(t), t->objectName(), priorityToString(t->priority()));
    }
} // ns
