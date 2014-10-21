/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "loghandler.h"
#include <QCoreApplication>
#include <QMetaMethod>

namespace BlackMisc
{
    Q_GLOBAL_STATIC(CLogHandler, g_handler)

    CLogHandler *CLogHandler::instance()
    {
        Q_ASSERT(! g_handler.isDestroyed());
        return g_handler;
    }

    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
    {
        CStatusMessage statusMessage(type, context, message);
        QMetaObject::invokeMethod(CLogHandler::instance(), "logLocalMessage", Q_ARG(BlackMisc::CStatusMessage, statusMessage));
    }

    void CLogHandler::install()
    {
        m_oldHandler = qInstallMessageHandler(messageHandler);
    }

    CLogHandler::CLogHandler()
    {
        // in case the first call to instance() is in a different thread
        moveToThread(QCoreApplication::instance()->thread());
    }

    CLogHandler::~CLogHandler()
    {
        qInstallMessageHandler(m_oldHandler);
    }

    CLogCategoryHandler *CLogHandler::handlerForCategoryPrefix(const QString &category)
    {
        Q_ASSERT(thread() == QThread::currentThread());

        if (! m_categoryPrefixHandlers.contains(category))
        {
            m_categoryPrefixHandlers[category] = new CLogCategoryHandler(this, m_enableFallThrough);

        }

        return m_categoryPrefixHandlers[category];
    }

    QList<CLogCategoryHandler *> CLogHandler::handlersForCategories(const CLogCategoryList &categories) const
    {
        QList<CLogCategoryHandler *> m_handlers;
        for (auto i = m_categoryPrefixHandlers.begin(); i != m_categoryPrefixHandlers.end(); ++i)
        {
            if (categories.anyStartWith(i.key()))
            {
                m_handlers.push_back(i.value());
            }
        }
        return m_handlers;
    }

    void CLogHandler::enableConsoleOutput(bool enable)
    {
        Q_ASSERT(thread() == QThread::currentThread());

        m_enableFallThrough = enable;
        for (auto *handler : m_categoryPrefixHandlers.values())
        {
            handler->enableConsoleOutput(enable);
        }
    }

    bool CLogHandler::isFallThroughEnabled(const QList<CLogCategoryHandler *> &handlers) const
    {
        for (const auto *handler : handlers)
        {
            if (handler->m_enableFallThrough != m_enableFallThrough)
            {
                return handler->m_enableFallThrough;
            }
        }
        return m_enableFallThrough;
    }

    void CLogHandler::logLocalMessage(const CStatusMessage &statusMessage)
    {
        logMessage(statusMessage);
        emit localMessageLogged(statusMessage);
    }

    void CLogHandler::logRemoteMessage(const CStatusMessage &statusMessage)
    {
        logMessage(statusMessage);
        emit remoteMessageLogged(statusMessage);
    }

    void CLogHandler::logMessage(const CStatusMessage &statusMessage)
    {
        collectGarbage();

        auto handlers = handlersForCategories(statusMessage.getCategories());

        if (isFallThroughEnabled(handlers))
        {
            QtMsgType type;
            QString category;
            QString message;
            statusMessage.toQtLogTriple(&type, &category, &message);
            m_oldHandler(type, QMessageLogContext(nullptr, 0, nullptr, qPrintable(category)), message);
        }

        for (auto *handler : handlers)
        {
            emit handler->messageLogged(statusMessage);
        }
    }

    void CLogHandler::collectGarbage()
    {
        auto newEnd = std::stable_partition(m_patternHandlers.begin(), m_patternHandlers.end(), [](const PatternPair &pair)
        {
            return ! pair.second->canBeDeleted();
        });
        std::for_each(newEnd, m_patternHandlers.end(), [](const PatternPair &pair) { pair.second->deleteLater(); });
        m_patternHandlers.erase(newEnd, m_patternHandlers.end());
    }

}
