/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "loghandler.h"
#include "algorithm.h"
#include "worker.h"
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

    CLogPatternHandler *CLogHandler::handlerForPattern(const CLogPattern &pattern)
    {
        Q_ASSERT(thread() == QThread::currentThread());

        auto finder = [ & ](const PatternPair &pair) { return pair.first == pattern; };
        auto comparator = [](const PatternPair &a, const PatternPair &b) { return a.first.isProperSubsetOf(b.first); };

        auto it = std::find_if(m_patternHandlers.begin(), m_patternHandlers.end(), finder);
        if (it == m_patternHandlers.end())
        {
            auto *handler = new CLogPatternHandler(this);
            topologicallySortedInsert(m_patternHandlers, PatternPair(pattern, handler), comparator);
            return handler;
        }
        else
        {
            return (*it).second;
        }
    }

    CLogPatternHandler *CLogHandler::handlerForCategory(const CLogCategory &category)
    {
        return handlerForPattern(CLogPattern::exactMatch(category));
    }

    QList<CLogPatternHandler *> CLogHandler::handlersForMessage(const CStatusMessage &message) const
    {
        QList<CLogPatternHandler *> m_handlers;
        for (const auto &pair : m_patternHandlers)
        {
            if (pair.first.match(message))
            {
                m_handlers.push_back(pair.second);
            }
        }
        return m_handlers;
    }

    bool CLogHandler::isFallThroughEnabled(const QList<CLogPatternHandler *> &handlers) const
    {
        for (const auto *handler : handlers)
        {
            if (! handler->m_inheritFallThrough)
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

        auto handlers = handlersForMessage(statusMessage);

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

    void CLogSubscriber::changeSubscription(const CLogPattern &pattern)
    {
        if (CLogHandler::instance()->thread() != QThread::currentThread())
        {
            Q_ASSERT(thread() == QThread::currentThread());
            singleShot(0, CLogHandler::instance()->thread(), this, [ = ]() { changeSubscription(pattern); });
            return;
        }

        unsubscribe();
        m_handler = CLogHandler::instance()->handlerForPattern(pattern);

        if (! m_inheritFallThrough)
        {
            m_handler->enableConsoleOutput(m_enableFallThrough);
        }
        connect(m_handler.data(), &CLogPatternHandler::messageLogged, this, &CLogSubscriber::ps_logMessage, Qt::DirectConnection);
    }

    void CLogSubscriber::unsubscribe()
    {
        if (CLogHandler::instance()->thread() != QThread::currentThread())
        {
            Q_ASSERT(thread() == QThread::currentThread());
            singleShot(0, CLogHandler::instance()->thread(), this, [ = ]() { unsubscribe(); });
            return;
        }

        if (m_handler)
        {
            if (! m_inheritFallThrough) { m_handler->inheritConsoleOutput(); }
            m_handler->disconnect(this);
        }
    }

    void CLogSubscriber::inheritConsoleOutput()
    {
        if (CLogHandler::instance()->thread() != QThread::currentThread())
        {
            Q_ASSERT(thread() == QThread::currentThread());
            singleShot(0, CLogHandler::instance()->thread(), this, [ = ]() { inheritConsoleOutput(); });
            return;
        }

        m_inheritFallThrough = true;
        if (m_handler)
        {
            m_handler->inheritConsoleOutput();
        }
    }

    void CLogSubscriber::enableConsoleOutput(bool enable)
    {
        if (CLogHandler::instance()->thread() != QThread::currentThread())
        {
            Q_ASSERT(thread() == QThread::currentThread());
            singleShot(0, CLogHandler::instance()->thread(), this, [ = ]() { enableConsoleOutput(enable); });
            return;
        }

        m_inheritFallThrough = false;
        m_enableFallThrough = enable;
        if (m_handler)
        {
            m_handler->enableConsoleOutput(enable);
        }
    }
}
