/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_LOGHANDLER_H
#define BLACKMISC_LOGHANDLER_H

//! \file

#include "logpattern.h"
#include "statusmessage.h"
#include <QObject>
#include <QMap>

namespace BlackMisc
{
    class CLogPatternHandler;

    /*!
     * Class for subscribing to log messages.
     */
    class CLogHandler : public QObject
    {
        Q_OBJECT

    public:
        //! \private Constructor.
        CLogHandler();

        //! \private Destructor.
        ~CLogHandler();

        //! Return pointer to the CLogHandler singleton.
        //! \warning This can not be called from within a plugin, because the returned instance will be wrong.
        static CLogHandler *instance();

        //! Tell the CLogHandler to install itself with qInstallMessageHandler.
        void install();

        //! Return a pattern handler for subscribing to all messages which match the given pattern.
        //! \warning This must only be called from the main thread.
        CLogPatternHandler *handlerForPattern(const CLogPattern &pattern);

    signals:
        //! Emitted when a message is logged in this process.
        void localMessageLogged(const BlackMisc::CStatusMessage &message);

        //! Emitted when a log message is relayed from a different process.
        void remoteMessageLogged(const BlackMisc::CStatusMessage &message);

    public slots:
        //! Called by our QtMessageHandler to log a message.
        void logLocalMessage(const BlackMisc::CStatusMessage &message);

        //! Called by the context to relay a message.
        void logRemoteMessage(const BlackMisc::CStatusMessage &message);

        //! Enable or disable the default Qt handler.
        void enableConsoleOutput(bool enable);

    private:
        void logMessage(const BlackMisc::CStatusMessage &message);
        void collectGarbage();
        QtMessageHandler m_oldHandler = nullptr;
        bool m_enableFallThrough = true;
        bool isFallThroughEnabled(const QList<CLogPatternHandler *> &handlers) const;
        using PatternPair = std::pair<CLogPattern, CLogPatternHandler *>;
        QList<PatternPair> m_patternHandlers;
        QList<CLogPatternHandler *> handlersForMessage(const CStatusMessage &message) const;
    };

    /*!
     * A class for subscribing to log messages which match particular patterns.
     * \see CLogHandler::handlerForPattern
     */
    class CLogPatternHandler : public QObject
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
        void enableConsoleOutput(bool enable) { Q_ASSERT(thread() == QThread::currentThread()); m_enableFallThrough = enable; }

    signals:
        /*!
         * Emitted when a message is logged which matches the relevant pattern.
         *
         * When all slots are disconnected from this signal, the CLogPatternHandler could be deleted.
         *
         * Note that if a message matches more that one handler's pattern, then this signal will be emitted for all of
         * those handlers, so if a slot is connected to all of them then it will be called multiple times. Use the methods
         * CStatusMessage::markAsHandledBy() and CStatusMessage::wasHandledBy() to detect this case in the slot and avoid
         * multiple handlings of the same message. Caveat: for this to work, the slot must take its argument by non-const
         * reference, and be connected by Qt::DirectConnection (i.e. the receiver is in the same thread as the CLogHandler).
         */
        void messageLogged(const CStatusMessage &message);

    private:
        friend class CLogHandler;
        CLogPatternHandler(QObject *parent, bool enableFallThrough) : QObject(parent), m_enableFallThrough(enableFallThrough) {}
        bool m_enableFallThrough;

        bool canBeDeleted()
        {
            static const auto signal = QMetaMethod::fromSignal(&CLogPatternHandler::messageLogged);
            return ! isSignalConnected(signal);
        }
    };
}

#endif
