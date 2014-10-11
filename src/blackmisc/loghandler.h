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

#include "statusmessage.h"
#include <QObject>
#include <QMap>

namespace BlackMisc
{
    class CLogCategoryHandler;

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

        //! Return a category handler for subscribing to all messages with a category starting with the given prefix.
        CLogCategoryHandler *handlerForCategoryPrefix(const QString &prefix);

        //! Enable or disable the default Qt handler.
        void enableConsoleOutput(bool enable);

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

    private:
        void logMessage(const BlackMisc::CStatusMessage &message);
        QtMessageHandler m_oldHandler = nullptr;
        bool m_enableFallThrough = true;
        bool isFallThroughEnabled(const QList<CLogCategoryHandler *> &handlers) const;
        QMap<QString, CLogCategoryHandler *> m_categoryPrefixHandlers;
        QList<CLogCategoryHandler *> handlersForCategory(const QString &category) const;
    };

    /*!
     * A class for subscribing to log messages in particular categories.
     * \sa CLogHandler::handlerForCategory
     */
    class CLogCategoryHandler : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * Enable or disable the default Qt handler for messages in relevant categories.
         * This can override the setting of the parent CLogHandler.
         */
        void enableConsoleOutput(bool enable) { m_enableFallThrough = enable; }

    signals:
        /*!
         * Emitted when a message is logged in a relevant category.
         */
        void messageLogged(const CStatusMessage &message);

    private:
        friend class CLogHandler;
        CLogCategoryHandler(QObject *parent, bool enableFallThrough) : QObject(parent), m_enableFallThrough(enableFallThrough) {}
        bool m_enableFallThrough;
    };
}

#endif
