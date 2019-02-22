/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackcore
 */

#include "expect.h"
#include "blackmisc/range.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QTest>
#include <QTimer>

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

    for (auto i : unitsCopy)
    {
        i->onDone([&](const ExpectUnit* u){ unitsCopy.remove(u); });
    }
    // toList is an easy way to make a temporary copy, needed because init might invalidate iterators
    for (auto i : unitsCopy.toList()) // clazy:exclude=container-anti-pattern,range-loop
    {
        i->init();
    }

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, [=, &unitsCopy]{
        reportTimeout(srcloc, unitsCopy);
        for (auto i : BlackMisc::as_const(unitsCopy))
        {
            i->onDone(nullptr); //paranoia
        }
        unitsCopy.clear();
        m_failed = true;
    });
    timer.start(timeout * 1000);

    while (! unitsCopy.isEmpty())
    {
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 10);
    }
}

void Expect::reportTimeout(const SourceLocation& srcloc, const QSet<const ExpectUnit*>& units)
{
    QString msg = "*** Timed Out ***";
    QString prefix = "\nwhile waiting for ";
    for (auto i = units.begin(); i != units.end(); ++i)
    {
        msg += prefix + (*i)->m_waitingFor;
        prefix = "\nand ";
    }
    QTest::qFail(qPrintable(msg), qPrintable(srcloc.file), srcloc.line);
}

//! \endcond

} //namespace
