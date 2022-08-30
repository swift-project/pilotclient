/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/loghandler.h"
#include "blackmisc/algorithm.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/threadutils.h"
#include "blackconfig/buildconfig.h"

#ifdef BLACK_USE_CRASHPAD
#include "crashpad/client/simulate_crash.h"
#endif

#include <QAbstractNativeEventFilter>
#include <QCoreApplication>
#include <QGlobalStatic>
#include <QMessageLogContext>
#include <QMetaMethod>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <algorithm>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

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
        if (type == QtFatalMsg && CLogHandler::instance()->thread() != QThread::currentThread())
        {
            // Fatal message means this thread is about to crash the application. A queued connection would be useless.
            // Blocking queued connection means we pause this thread just long enough to let the main thread handle the message.
            QMetaObject::invokeMethod(CLogHandler::instance(), [ & ] { messageHandler(type, context, message); }, Qt::BlockingQueuedConnection);
            return;
        }
#if defined(Q_CC_MSVC) && defined(QT_NO_DEBUG)
        if (type == QtFatalMsg)
        {
            struct EventFilter : public QAbstractNativeEventFilter
            {
                // Prevent Qt from handling Windows Messages while the messagebox is open
                virtual bool nativeEventFilter(const QByteArray &, void *message, long *result) override
                {
                    auto msg = static_cast<MSG *>(message);
                    *result = DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
                    return true;
                }
            };
            EventFilter ef;
            qApp->installNativeEventFilter(&ef);
            MessageBoxW(nullptr, message.toStdWString().c_str(), nullptr, MB_OK); // display assert dialog in release build
            qApp->removeNativeEventFilter(&ef);
#   if defined(BLACK_USE_CRASHPAD)
            CRASHPAD_SIMULATE_CRASH(); // workaround inability to catch __fastfail
#   endif
        }
#endif
        QMetaObject::invokeMethod(CLogHandler::instance(), [statusMessage = CStatusMessage(type, context, message)]
        {
            CLogHandler::instance()->logLocalMessage(statusMessage);
        });
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

        auto finder = [ & ](const PatternPair & pair) { return pair.first == pattern; };
        auto comparator = [](const PatternPair & a, const PatternPair & b) { return a.first.isProperSubsetOf(b.first); };

        auto it = std::find_if(m_patternHandlers.cbegin(), m_patternHandlers.cend(), finder);
        if (it == m_patternHandlers.cend())
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

    void CLogHandler::logLocalMessage(const CStatusMessage &i_statusMessage)
    {
        using namespace BlackConfig;
        CStatusMessage statusMessage = i_statusMessage;
        if (!CBuildConfig::isLocalDeveloperDebugBuild() && CLogPattern::empty().withSeverity(CStatusMessage::SeverityError).match(statusMessage))
        {
            // 99% this is a complex Qt implementation warning generated by qErrnoWarning, so downgrade its severity
            statusMessage.setSeverity(CStatusMessage::SeverityDebug);
        }

        if (!CBuildConfig::isLocalDeveloperDebugBuild() && CLogPattern::exactMatch("default").withSeverity(CStatusMessage::SeverityWarning).match(statusMessage))
        {
            // All Qt warnings

            // demoted because caused by airline icons, we would need to re-create literally dozens of these images
            if (statusMessage.getMessage().startsWith(QStringLiteral("libpng warning"))) { statusMessage.setSeverity(CStatusMessage::SeverityDebug); }

            // demoted, because in some swift APPs some options can be ignored
            else if (statusMessage.getMessage().startsWith(QStringLiteral("QCommandLineParser: option not defined"))) { statusMessage.setSeverity(CStatusMessage::SeverityDebug); }
        }

        auto bucket = m_tokenBuckets.find(statusMessage);
        if (bucket == m_tokenBuckets.end()) { bucket = m_tokenBuckets.insert(statusMessage, { { 5, 1000, 1 }, 0 }); }
        if (! bucket->first.tryConsume())
        {
            bucket->second++;
            return;
        }
        if (bucket->second > 0)
        {
            auto copy = statusMessage;
            copy.appendMessage(QStringLiteral(" (+%1 identical messages)").arg(bucket->second));
            bucket->second = 0;

            logMessage(copy);
            emit localMessageLogged(copy);
            return;
        }

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
        auto it = std::find_if(m_patternHandlers.begin(), m_patternHandlers.end(), [handler](const PatternPair & pair)
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
