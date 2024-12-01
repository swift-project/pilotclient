// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_LOGHANDLER_H
#define SWIFT_MISC_LOGHANDLER_H

#include <atomic>
#include <utility>

#include <QHash>
#include <QList>
#include <QMetaMethod>
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QThread>
#include <QTimer>
#include <QtGlobal>
#include <QtMessageHandler>

#include "misc/logcategory.h"
#include "misc/logpattern.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/tokenbucket.h"

namespace swift::misc
{
    class CLogPatternHandler;

    /*!
     * Class for subscribing to log messages.
     */
    class SWIFT_MISC_EXPORT CLogHandler : public QObject
    {
        Q_OBJECT

    public:
        //! \private Constructor.
        CLogHandler();

        //! \private Destructor.
        virtual ~CLogHandler();

        //! Return pointer to the CLogHandler singleton.
        //! \warning This can not be called from within a plugin, because the returned instance will be wrong.
        static CLogHandler *instance();

        //! Tell the CLogHandler to install itself with qInstallMessageHandler.
        void install(bool skipIfAlreadyInstalled = false);

        //! Return a pattern handler for subscribing to all messages which match the given pattern.
        //! \warning This must only be called from the main thread.
        CLogPatternHandler *handlerForPattern(const CLogPattern &pattern);

        //! Return a pattern handler for subscribing to all messages which contain the given category.
        //! \warning This must only be called from the main thread.
        CLogPatternHandler *handlerForCategory(const CLogCategory &category);

        //! Return a pattern handler for subscribing to all validation warnings and errors.
        //! \warning This must only be called from the main thread.
        CLogPatternHandler *handlerForValidation()
        {
            return handlerForPattern(CLogPattern::exactMatch(CLogCategories::validation())
                                         .withSeverityAtOrAbove(CStatusMessage::SeverityWarning));
        }

        //! Returns all log patterns for which there are currently subscribed log pattern handlers.
        QList<CLogPattern> getAllSubscriptions() const;

    signals:
        //! Emitted when a message is logged in this process.
        void localMessageLogged(const swift::misc::CStatusMessage &message);

        //! Emitted when a log message is relayed from a different process.
        void remoteMessageLogged(const swift::misc::CStatusMessage &message);

        //! Emitted when an object subscribes to a pattern of log messages.
        void subscriptionAdded(const swift::misc::CLogPattern &pattern);

        //! Emitted when an object unsubscribes from a pattern of log messages.
        void subscriptionRemoved(const swift::misc::CLogPattern &pattern);

    public slots:
        //! Called by our QtMessageHandler to log a message.
        void logLocalMessage(const swift::misc::CStatusMessage &message);

        //! Called by the context to relay a message.
        void logRemoteMessage(const swift::misc::CStatusMessage &message);

        //! Enable or disable the default Qt handler.
        void enableConsoleOutput(bool enable);

    private:
        friend class CLogPatternHandler;
        void logMessage(const swift::misc::CStatusMessage &message);
        QtMessageHandler m_oldHandler = nullptr;
        bool m_enableFallThrough = true;
        bool isFallThroughEnabled(const QList<CLogPatternHandler *> &handlers) const;
        using PatternPair = std::pair<CLogPattern, CLogPatternHandler *>;
        QList<PatternPair> m_patternHandlers;
        QList<CLogPatternHandler *> handlersForMessage(const CStatusMessage &message) const;
        void removePatternHandler(CLogPatternHandler *);
        QHash<CStatusMessage, std::pair<CTokenBucket, int>> m_tokenBuckets;
    };

    /*!
     * A class for subscribing to log messages which match particular patterns.
     * \see CLogHandler::handlerForPattern
     */
    class SWIFT_MISC_EXPORT CLogPatternHandler : public QObject
    {
        Q_OBJECT

    public slots:
        /*!
         * Enable or disable the default Qt handler for messages which match the relevant pattern.
         *
         * The setting of this property in one CLogPatternHandler can override the setting in another
         * CLogPatternHandler or the base CLogHandler, if this handler's pattern is a subset of the
         * other handler's pattern. Which is to say, more specific patterns can override less specific patterns.
         */
        void enableConsoleOutput(bool enable)
        {
            Q_ASSERT(thread() == QThread::currentThread());
            m_inheritFallThrough = false;
            m_enableFallThrough = enable;
            m_subscriptionNeedsUpdate = true;
        }

        /*!
         * The policy of whether to enable or disable the default Qt handler for messages which match
         * the relevant pattern will be inherited from the base CLogHandler, or from another CLogPatternHandler
         * which matches a superset of the messages which this one matches.
         *
         * This is the default, but can be used to reverse the effect of calling enableConsoleOutput.
         */
        void inheritConsoleOutput()
        {
            Q_ASSERT(thread() == QThread::currentThread());
            m_inheritFallThrough = true;
            m_subscriptionNeedsUpdate = true;
        }

    signals:
        /*!
         * Emitted when a message is logged which matches the relevant pattern.
         *
         * When all slots are disconnected from this signal, the CLogPatternHandler could be deleted.
         *
         * Note that if a message matches more that one handler's pattern, then this signal will be emitted for all of
         * those handlers, so if a slot is connected to all of them then it will be called multiple times. Use the
         * methods CStatusMessage::markAsHandledBy() and CStatusMessage::wasHandledBy() to detect this case in the slot
         * and avoid multiple handlings of the same message. Caveat: for this to work, the slot must take its argument
         * by non-const reference, and be connected by Qt::DirectConnection (i.e. the receiver is in the same thread as
         * the CLogHandler).
         */
        void messageLogged(const swift::misc::CStatusMessage &message);

    public:
        /*!
         * Convenience method to connect the messageLogged signal to a slot in the given receiver object.
         */
        template <typename T, typename F>
        QMetaObject::Connection subscribe(T *receiver, F slot)
        {
            return connect(this, &CLogPatternHandler::messageLogged, receiver, slot);
        }

        /*!
         * Convenience method to connect the messageLogged signal to a functor.
         */
        template <typename F>
        QMetaObject::Connection subscribe(F slot)
        {
            return connect(this, &CLogPatternHandler::messageLogged, slot);
        }

    protected:
        //! \copydoc QObject::connectNotify
        virtual void connectNotify(const QMetaMethod &signal) override
        {
            if (signal == QMetaMethod::fromSignal(&CLogPatternHandler::messageLogged))
            {
                m_subscriptionNeedsUpdate = true;
            }
        }

        //! \copydoc QObject::disconnectNotify
        virtual void disconnectNotify(const QMetaMethod &signal) override
        {
            if (signal == QMetaMethod::fromSignal(&CLogPatternHandler::messageLogged))
            {
                m_subscriptionNeedsUpdate = true;
            }
        }

    private:
        friend class CLogHandler;
        CLogPatternHandler(CLogHandler *parent, const CLogPattern &pattern);
        CLogHandler *m_parent = nullptr;
        CLogPattern m_pattern;
        bool m_inheritFallThrough = true;
        bool m_enableFallThrough = true;
        bool m_isSubscribed = false;
        std::atomic<bool> m_subscriptionNeedsUpdate { false };
        QTimer m_subscriptionUpdateTimer;
        void updateSubscription();
    };

    /*!
     * A helper class for subscribing to log messages matching a particular pattern, with the ability to
     * change the pattern at runtime.
     *
     * Also provides a thread-safe API for interacting with the CLogHandler.
     */
    class SWIFT_MISC_EXPORT CLogSubscriber : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor, for when you're not interested in messages and just want to control the console output.
        CLogSubscriber(QObject *parent = nullptr) : QObject(parent) {}

        //! Construct a subscriber which forwards messages to the given slot of parent.
        template <typename T, typename F>
        CLogSubscriber(T *parent, F slot) : QObject(parent)
        {
            QObject::connect(this, &CLogSubscriber::ps_messageLogged, parent, slot);
        }

        //! Change the pattern which you want to subscribe to.
        //! \threadsafe If not called from the main thread, it will run asynchronously.
        void changeSubscription(const CLogPattern &pattern);

        //! Unsubscribe from all messages.
        //! \threadsafe If not called from the main thread, it will run asynchronously.
        void unsubscribe();

        //! \copydoc CLogPatternHandler::enableConsoleOutput
        //! \threadsafe If not called from the main thread, it will run asynchronously.
        void enableConsoleOutput(bool enable);

        //! \copydoc CLogPatternHandler::inheritConsoleOutput
        //! \threadsafe If not called from the main thread, it will run asynchronously.
        void inheritConsoleOutput();

    signals:
        //! \private
        void ps_messageLogged(const swift::misc::CStatusMessage &message);

    private slots:
        void ps_logMessage(const swift::misc::CStatusMessage &message) { emit ps_messageLogged(message); }

    private:
        QPointer<CLogPatternHandler> m_handler;
        bool m_inheritFallThrough = true;
        bool m_enableFallThrough = true;
    };
} // namespace swift::misc

#endif // SWIFT_MISC_LOGHANDLER_H
