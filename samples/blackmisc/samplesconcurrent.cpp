/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesconcurrent.h"
#include "blackmisc/algorithm.h"
#include <QDebug>
#include <QGuiApplication>
#include <QtConcurrent/QtConcurrent>
#include <QString>

using namespace BlackMisc;

namespace BlackMiscTest
{

    int CSamplesConcurrent::samples(const QString &type, QTextStream &out, QTextStream &in)
    {
        Q_UNUSED(in);
        Q_UNUSED(type);

        out << "Main thread id: " << QThread::currentThreadId() << endl;
        CThreadOutput b1("b1", QGuiApplication::instance());
        b1.start();
        CThreadOutput b2("b2", QGuiApplication::instance());
        b2.start();
        CThreadOutput b3("b3", QGuiApplication::instance());
        b3.start();
        QThread::msleep(10 * 1000);
        b1.stop();
        b2.stop();
        b3.stop();
        QThread::msleep(1 * 1000); // allow to stop

        out << "Main thread id: " << QThread::currentThreadId() << endl;
        CConcurrentOutput c1("c1");
        QtConcurrent::run(&c1, &CConcurrentOutput::doWork);
        CConcurrentOutput c2("c2");
        QtConcurrent::run(&c2, &CConcurrentOutput::doWork);
        CConcurrentOutput c3("c3");
        QtConcurrent::run(&c3, &CConcurrentOutput::doWork);
        QThread::msleep(10 * 1000);
        c1.stop();
        c2.stop();
        // stop 2, but then wait and see whats happening
        out << "stopped c1, c2" << endl;
        QThread::msleep(5 * 1000);
        out << "stopped c3" << endl;
        c3.stop();
        QThread::msleep(1 * 1000); // allow to stop

        return 0;
    }

    CThreadOutput::CThreadOutput(const QString &name, QObject *parent) :
        CContinuousWorker(parent, name)
    { }

    void CThreadOutput::doWork()
    {
        QTextStream out(stdout);
        while (m_run)
        {
            out << this->objectName() << " worker id: " << QThread::currentThreadId() << endl;
            QThread::msleep(1000);
        }
    }

    void CThreadOutput::stop()
    {
        m_run = false;
    }

    void CThreadOutput::initialize()
    {
        this->doWork();
    }

    CConcurrentOutput::CConcurrentOutput(const QString &name, QObject *parent) :
        QObject(parent), m_name(name)
    { }

    void CConcurrentOutput::stop()
    {
        m_run = false;
    }

    void CConcurrentOutput::doWork()
    {
        QTextStream out(stdout);
        while (m_run)
        {
            out << m_name << " worker id: " << QThread::currentThreadId() << endl;
            QThread::msleep(1000);
        }
    }

} // namespace
