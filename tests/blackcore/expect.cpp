/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "expect.h"
#include <QCoreApplication>
#include <QTimer>
#include <QtTest/QtTest>

namespace BlackCoreTest
{

void ExpectUnit::wait(int timeout)
{
    m_parent->wait(m_srcloc, timeout, *this);
}

/* Connect slot for the first expectation, then send all queued signals.
 */
void ExpectUnit::init() const
{
    m_nextExpect = m_expects.begin();
    next();
    for (auto i = m_sends.begin(); i != m_sends.end(); ++i)
    {
        (*i)();
    }
}

/* Connect slot for the next expectation, or call the onDone() callback if that was the last one.
 * Each expectation slot calls this function, so each successful expectation connects the next one.
 */
void ExpectUnit::next() const
{
    m_guard.cleanup();

    if (m_nextExpect == m_expects.end())
    {
        if (m_onDone)
        {
            m_onDone(this);
        }
    }
    else
    {
        (*m_nextExpect)();
        ++m_nextExpect;
    }
}

/* For each unit queued by an overloaded wait(), setup its onDone() callback and call its init() method.
 * The onDone() callback removes the unit from the queue. When all units have been removed from the queue,
 * then the expectation has succeeded. If the timer times out before that happens, then the expectation
 * has failed. Failures cause subsequent calls to wait() to do nothing.
 */
void Expect::wait(const SourceLocation& srcloc, int timeout)
{
    auto unitsCopy = m_units;
    m_units.clear();

    if (m_failed)
    {
        return;
    }

    for (auto i = unitsCopy.begin(); i != unitsCopy.end(); ++i)
    {
        (*i)->onDone([&](const ExpectUnit* u){ unitsCopy.remove(u); });
        (*i)->init();
    }

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, [=, &unitsCopy]{
        for (auto i = unitsCopy.begin(); i != unitsCopy.end(); ++i)
        {
            (*i)->onDone(nullptr); //paranoia
        }
        unitsCopy.clear();
        QTest::qFail("*** Timed Out ***", qPrintable(srcloc.file), srcloc.line);
        m_failed = true;
    });
    timer.start(timeout * 1000);

    while (! unitsCopy.isEmpty())
    {
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 10);
    }
}

} //namespace BlackCoreTest