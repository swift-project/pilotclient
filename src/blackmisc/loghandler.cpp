/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/algorithm.h"
#include "blackmisc/compare.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/worker.h"

#include <QCoreApplication>
#include <QGlobalStatic>
#include <QMessageLogContext>
#include <QMetaMethod>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <algorithm>
#include <tuple>

namespace BlackMisc
{
    Q_GLOBAL_STATIC(CLogHandler, g_handler)

    CLogHandler *CLogHandler::instance()
    {
        Q_ASSERT(! g_handler.isDestroyed());
        return g_handler;
    }

    //! Qt message handler
    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
    {
        CStatusMessage statusMessage(type, context, message);
        QMetaObject::invokeMethod(CLogHandler::instance(), "logLocalMessage", Q_ARG(BlackMisc::CStatusMessage, statusMessage));
    }

    void CLogHandler::install(bool skipIfAlreadyInstalled)
    {
        if (skipIfAlreadyInstalled && m_oldHandler) { return; }
        Q_ASSERT_X(!m_oldHandler, Q_FUNC_INFO, "Re-installing the log handler should be avoided");
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
            auto *handler = new CLogPatternHandler(this, pattern);
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

    void BlackMisc::CLogHandler::enableConsoleOutput(bool enable)
    {
        Q_ASSERT_X(m_oldHandler, Q_FUNC_INFO, "Install the log handler before using it");
        Q_ASSERT_X(thread() == QThread::currentThread(), Q_FUNC_INFO, "Wrong thread");
        m_enableFallThrough = enable;
    }

    void CLogHandler::logMessage(const CStatusMessage &statusMessage)
    {
        const auto handlers = handlersForMessage(statusMessage);

        if (isFallThroughEnabled(handlers))
        {
            Q_ASSERT_X(m_oldHandler, Q_FUNC_INFO, "Handler must be installed");
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

    void CLogHandler::removePatternHandler(CLogPatternHandler *handler)
    {
        auto it = std::find_if(m_patternHandlers.begin(), m_patternHandlers.end(), [handler](const PatternPair &pair)
        {
            return pair.second == handler;
        });
        if (it != m_patternHandlers.end())
        {
            it->second->deleteLater();
            m_patternHandlers.erase(it);
        }
    }

    QList<CLogPattern> CLogHandler::getAllSubscriptions() const
    {
        QList<CLogPattern> result;
        for (const auto &pair : m_patternHandlers)
        {
            if (pair.second->isSignalConnected(QMetaMethod::fromSignal(&CLogPatternHandler::messageLogged)))
            {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    CLogPatternHandler::CLogPatternHandler(CLogHandler *parent, const CLogPattern &pattern) :
        QObject(parent), m_parent(parent), m_pattern(pattern)
    {
        connect(&m_subscriptionUpdateTimer, &QTimer::timeout, this, &CLogPatternHandler::updateSubscription);
        m_subscriptionUpdateTimer.start(1);
    }

    void CLogPatternHandler::updateSubscription()
    {
        if (m_subscriptionNeedsUpdate)
        {
            m_subscriptionNeedsUpdate = false;
            bool isSubscribed = isSignalConnected(QMetaMethod::fromSignal(&CLogPatternHandler::messageLogged));

            if (isSubscribed != m_isSubscribed)
            {
                m_isSubscribed = isSubscribed;
                if (m_isSubscribed)
                {
                    emit m_parent->subscriptionAdded(m_pattern);
                }
                else
                {
                    emit m_parent->subscriptionRemoved(m_pattern);
                }
            }

            if (m_inheritFallThrough && ! m_isSubscribed)
            {
                m_parent->removePatternHandler(this);
            }
        }
    }

    void CLogSubscriber::changeSubscription(const CLogPattern &pattern)
    {
        if (CLogHandler::instance()->thread() != QThread::currentThread())
        {
            Q_ASSERT(thread() == QThread::currentThread());
            singleShot(0, CLogHandler::instance(), [pattern, self = QPointer<CLogSubscriber>(this)]() { if (self) { self->changeSubscription(pattern); } });
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
            singleShot(0, CLogHandler::instance(), [self = QPointer<CLogSubscriber>(this)]() { if (self) { self->unsubscribe(); } });
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
            singleShot(0, CLogHandler::instance(), [self = QPointer<CLogSubscriber>(this)]() { if (self) { self->inheritConsoleOutput(); } });
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
            singleShot(0, CLogHandler::instance(), [enable, self = QPointer<CLogSubscriber>(this)]() { if (self) { self->enableConsoleOutput(enable); } });
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

//! \endcond
