/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORETEST_EXPECT_H
#define BLACKCORETEST_EXPECT_H

#include <QMetaMethod>
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QString>
#include <QVector>
#include <QtGlobal>
#include <functional>

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackcore
 */

namespace BlackCoreTest
{
    class Expect;

    /*!
     * Class representing a position in a source code file, for error reporting. Used in combination with the SOURCE_LOCATION macro.
     */
    class SourceLocation
    {
    public:
        SourceLocation(const QString &f, int ln) : file(f), line(ln) {} //!< Constructor
        const QString file; //!< Source code filename
        const int line; //!< Line number
    };

//! Evaluates to an instance of SourceLocation representing the position where the macro is used.
#define SOURCE_LOCATION (BlackCoreTest::SourceLocation(__FILE__, __LINE__))

    /*!
     * RAII class for Qt signal/slot connections. All connections are closed when object is destroyed.
     */
    class ConnectGuard
    {
    public:
        //! Constructor.
        ConnectGuard() {}

        //! Destructor.
        ~ConnectGuard() { cleanup(); }

        //! Add a connection to the object..
        ConnectGuard &operator+= (const QMetaObject::Connection &conn) { m_conns += conn; return *this; }

        //! Disconnect and remove all stored connections.
        void cleanup() { for (auto i = m_conns.cbegin(); i != m_conns.cend(); ++i) QObject::disconnect(*i); m_conns.clear(); }

        //! Copying is only allowed when there are no connections stored.
        //! @{
        ConnectGuard(const ConnectGuard &other) { Q_ASSERT(other.m_conns.isEmpty()); Q_UNUSED(other); }
        ConnectGuard &operator= (const ConnectGuard &other) { Q_ASSERT(other.m_conns.isEmpty()); Q_UNUSED(other); return *this; }
        //! @}

    private:
        QVector<QMetaObject::Connection> m_conns;
    };

    /*!
     * One unit of expectation, as returned by Expect::unit().
     *
     * Contains a sequence of signals to send, and a sequence of signals that are expected to be received in reply.
     */
    class ExpectUnit
    {
    public:
        /*!
         * Adds a signal to the list of signals to send, with optional arguments.
         * \param slot A pointer-to-member-function of the subject class.
         * \return this object, so methods can be chained.
         */
        template <class F> ExpectUnit &send(F slot) { m_sends.push_back(std::bind(slot, subject<F>())); return *this; }

        /*!
         * \copydoc send(F)
         * \param arg1
         */
        template <class F, class T1> ExpectUnit &send(F slot, T1 arg1) { m_sends.push_back(std::bind(slot, subject<F>(), arg1)); return *this; }

        /*!
         * \copydoc send(F,T1)
         * \param arg2
         */
        template <class F, class T1, class T2> ExpectUnit &send(F slot, T1 arg1, T2 arg2) { m_sends.push_back(std::bind(slot, subject<F>(), arg1, arg2)); return *this; }

        /*!
         * \copydoc send(F,T1,T2)
         * \param arg3
         */
        template <class F, class T1, class T2, class T3> ExpectUnit &send(F slot, T1 arg1, T2 arg2, T3 arg3) { m_sends.push_back(std::bind(slot, subject<F>(), arg1, arg2, arg3)); return *this; }

        /*!
         * \copydoc send(F,T1,T2,T3)
         * \param arg4
         */
        template <class F, class T1, class T2, class T3, class T4> ExpectUnit &send(F slot, T1 arg1, T2 arg2, T3 arg3, T4 arg4) { m_sends.push_back(std::bind(slot, subject<F>(), arg1, arg2, arg3, arg4)); return *this; }

        /*!
         * \copydoc send(F,T1,T2,T3,T4)
         * \param arg5
         */
        template <class F, class T1, class T2, class T3, class T4, class T5> ExpectUnit &send(F slot, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) { m_sends.push_back(std::bind(slot, subject<F>(), arg1, arg2, arg3, arg4, arg5)); return *this; }

        /*!
         * Adds a signal to the list of signals which are expects to be received.
         * \param signal A pointer-to-member-function of the subject class.
         * \param slot A callable object which will be connected to the signal, e.g. to validate the signal arguments.
         * \return this object, so methods can be chained.
         */
        template <class F1, class F2> ExpectUnit &expect(F1 signal, F2 slot)
        {
            auto subj = subject<F1>();
            auto next = [ = ] { this->next(); };
            m_expects.push_back([ = ]
            {
                m_waitingFor = subj->metaObject()->className();
                m_waitingFor += "::";
                m_waitingFor += QMetaMethod::fromSignal(signal).name();
                m_guard += QObject::connect(subj, signal, slot);
                m_guard += QObject::connect(subj, signal, next);
            });
            return *this;
        }

        /*!
         * Adds a signal to the list of signals which are expected to be received.
         * \param signal A pointer-to-member-function of the subject class.
         * \return this object, so methods can be chained.
         */
        template <class F> ExpectUnit &expect(F signal) { return expect(signal, [] {}); }

        /*!
         * Sends all the queued signals, then waits for each of the expected signals in sequence.
         * If the full sequence of expected signals is not received, calls QTest::qFail().
         * \param timeout Time to wait in seconds. Qt event queue is processed when waiting.
         */
        void wait(int timeout);

    private:
        friend class Expect;

        ExpectUnit(Expect *parent, QObject *subject, const SourceLocation &srcloc) : m_parent(parent), m_subject(subject), m_srcloc(srcloc) {}

        void init() const;
        void next() const;

        QPointer<Expect> m_parent;
        QPointer<QObject> m_subject;
        SourceLocation m_srcloc;
        QString m_waitingFor;
        QVector<std::function<void()>> m_sends;
        QVector<std::function<void()>> m_expects;
        mutable QVector<std::function<void()>>::const_iterator m_nextExpect;
        mutable ConnectGuard m_guard;

        mutable std::function<void(const ExpectUnit *)> m_onDone;
        void onDone(const std::function<void(const ExpectUnit *)> &callback) const { m_onDone = callback; }

        // Helper traits class. Given T is a pointer-to-member-of-U, ClassOf<T>::type is U.
        template <class T> struct ClassOf;

        template <class U, class R> struct ClassOf<R U::*> { using type = U; };

        // Given T is a pointer-to-member-of-U, subject<T>() returns Expect's subject casted to U*.
        template <class T> typename ClassOf<T>::type *subject() const { return qobject_cast<typename ClassOf<T>::type *>(m_subject.data()); }
    };

    /*!
     * Class for writing unit tests in which signals are sent to some object, and other signals are expected to be received in reply.
     * Loosely modelled on the C library libexpect.
     *
     * Commonly used in combination with the macros EXPECT_UNIT or EXPECT_WAIT.
     */
    class Expect : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * Constructor.
         * \param testSubject The object to be tested.
         */
        explicit Expect(QObject *testSubject) : m_subject(testSubject), m_failed(false) {}

        /*!
         * Add a single expectation that will remain for the lifetime of the object.
         * \param signal A pointer-to-member-function of the subject class.
         * \param slot A callable object which will be connected to the signal, e.g. to validate the signal arguments.
         */
        template <class F1, class F2> void always(F1 signal, F2 slot)
        {
            m_guard += QObject::connect(m_subject, signal, slot);
        }

        /*!
         * Returns a new unit of expectation for the subject. Commonly called via the EXPECT_UNIT macro.
         * \param srcloc Represents the caller's location in the source code, for error reporting.
         */
        ExpectUnit unit(const SourceLocation &srcloc = SOURCE_LOCATION) { return ExpectUnit(this, m_subject, srcloc); }

        /*!
         * Allows two or more units of expectation to be waited on simultaneously. Commonly valled via the EXPECT_WAIT macro.
         * \param srcloc Represents the caller's location in the source code, for error reporting.
         * \param timeout Time to wait in seconds. Qt event queue is processed when waiting.
         * \param u1
         */
        void wait(const SourceLocation &srcloc, int timeout, const ExpectUnit &u1) { m_units.insert(&u1); wait(srcloc, timeout); }

        /*!
         * \copydoc wait(const SourceLocation&,int,const ExpectUnit&)
         * \param u2
         */
        void wait(const SourceLocation &srcloc, int timeout, const ExpectUnit &u1, const ExpectUnit &u2) { m_units.insert(&u1); m_units.insert(&u2); wait(srcloc, timeout); }

        /*!
         * \copydoc wait(const SourceLocation&,int,const ExpectUnit&,const ExpectUnit&)
         * \param u3
         */
        void wait(const SourceLocation &srcloc, int timeout, const ExpectUnit &u1, const ExpectUnit &u2, const ExpectUnit &u3) { m_units.insert(&u1); m_units.insert(&u2); m_units.insert(&u3); wait(srcloc, timeout); }

        /*!
         * \copydoc wait(const SourceLocation&,int,const ExpectUnit&,const ExpectUnit&,const ExpectUnit&)
         * \param u4
         */
        void wait(const SourceLocation &srcloc, int timeout, const ExpectUnit &u1, const ExpectUnit &u2, const ExpectUnit &u3, const ExpectUnit &u4) { m_units.insert(&u1); m_units.insert(&u2); m_units.insert(&u3); m_units.insert(&u4); wait(srcloc, timeout); }

        /*!
         * \copydoc wait(const SourceLocation&,int,const ExpectUnit&,const ExpectUnit&,const ExpectUnit&,const ExpectUnit&)
         * \param u5
         */
        void wait(const SourceLocation &srcloc, int timeout, const ExpectUnit &u1, const ExpectUnit &u2, const ExpectUnit &u3, const ExpectUnit &u4, const ExpectUnit &u5) { m_units.insert(&u1); m_units.insert(&u2); m_units.insert(&u3); m_units.insert(&u4); m_units.insert(&u5); wait(srcloc, timeout); }

    private:
        friend class ExpectUnit;

        void wait(const SourceLocation &srcloc, int timeout);
        void reportTimeout(const SourceLocation &srcloc, const QSet<const ExpectUnit *> &units);

        QPointer<QObject> m_subject;
        QSet<const ExpectUnit *> m_units;
        ConnectGuard m_guard;
        bool m_failed;
    };

    /*!
     * Wrapper for Expect::unit() which fills in the source location parameter.
     * Returns a new unit of expectation for the subject.
     * \param EXP Instance of Expect on which to call unit().
     */
#define EXPECT_UNIT(EXP) ((EXP).unit(SOURCE_LOCATION))

    /*!
     * Wrapper for Expect::wait() which fills in the source location parameter.
     * Allows two or more units of expectation to be waited on simultaneously.
     * \param EXP Instance of Expect on which to call wait().
     * \param TIME Time to wait in seconds. Qt event queue is processed when waiting.
     * \param U1, U2
     */
#define EXPECT_WAIT_2(EXP, TIME, U1, U2) ((EXP).wait(SOURCE_LOCATION, (TIME), (U1), (U2)))

    /*!
     * \copydoc EXPECT_WAIT_2(EXP,TIME,U1,U2)
     * \param U3
     */
#define EXPECT_WAIT_3(EXP, TIME, U1, U2, U3) ((EXP).wait(SOURCE_LOCATION, (TIME), (U1), (U2), (U3)))

    /*!
     * \copydoc EXPECT_WAIT_3(EXP,TIME,U1,U2,U3)
     * \param U4
     */
#define EXPECT_WAIT_4(EXP, TIME, U1, U2, U3, U4) ((EXP).wait(SOURCE_LOCATION, (TIME), (U1), (U2), (U3), (U4)))

    /*!
     * \copydoc EXPECT_WAIT_4(EXP,TIME,U1,U2,U3,U4)
     * \param U5
     */
#define EXPECT_WAIT_5(EXP, TIME, U1, U2, U3, U4, U5) ((EXP).wait(SOURCE_LOCATION, (TIME), (U1), (U2), (U3), (U4), (U5)))

} // ns

//! \endcond

#endif // guard
