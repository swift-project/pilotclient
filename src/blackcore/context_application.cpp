#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackcore/context_application_proxy.h"
#include "blackcore/context_settings.h"
#include "blackcore/input_manager.h"
#include "blackmisc/statusmessage.h"
#include <QCoreApplication>
#include <QThread>

using namespace BlackMisc;
using namespace BlackMisc::Settings;

namespace BlackCore
{

    QList<IContextApplication *> IContextApplication::s_contexts;
    QtMessageHandler IContextApplication::s_oldHandler = nullptr;

    IContextApplication *IContextApplication::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextApplication(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextApplicationProxy(BlackCore::CDBusServer::ServiceName, conn, mode, parent);
        default:
            qFatal("Always initialize an application context!");
            return nullptr;
        }
    }

    IContextApplication::RedirectionLevel IContextApplication::getOutputRedirectionLevel() const
    {
        QReadLocker(&this->m_lock);
        return this->m_outputRedirectionLevel;
    }

    void IContextApplication::setOutputRedirectionLevel(IContextApplication::RedirectionLevel redirectionLevel)
    {
        QWriteLocker(&this->m_lock);
        this->m_outputRedirectionLevel = redirectionLevel;
    }

    IContextApplication::RedirectionLevel IContextApplication::getStreamingForRedirectedOutputLevel() const
    {
        QReadLocker(&this->m_lock);
        return this->m_redirectedOutputRedirectionLevel;
    }

    /*
     * Constructor
     */
    IContextApplication::IContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        CContext(mode, runtime), m_outputRedirectionLevel(IContextApplication::RedirectNone), m_redirectedOutputRedirectionLevel(IContextApplication::RedirectNone)
    {
        if (IContextApplication::s_contexts.isEmpty())
        {
            IContextApplication::s_oldHandler = qInstallMessageHandler(IContextApplication::messageHandlerDispatch);
        }
        IContextApplication::s_contexts.append(this);

        changeSettings(IContextSettings::SettingsHotKeys);
    }

    /*
     * Output data from redirect signal
     */
    void IContextApplication::setStreamingForRedirectedOutputLevel(RedirectionLevel redirectionLevel)
    {
        QWriteLocker(&this->m_lock);
        disconnect(this, &IContextApplication::redirectedOutput, this, &IContextApplication::streamRedirectedOutput);
        if (redirectionLevel != RedirectNone)
        {
            connect(this, &IContextApplication::redirectedOutput, this, &IContextApplication::streamRedirectedOutput);
        }
        this->m_redirectedOutputRedirectionLevel = redirectionLevel;
    }

    /*
     * Reset output redirection
     */
    void IContextApplication::resetOutputRedirection()
    {
        qInstallMessageHandler(0);
    }

    void IContextApplication::changeSettings(uint typeValue)
    {
        auto type = static_cast<IContextSettings::SettingsType>(typeValue);
        switch (type)
        {
        case IContextSettings::SettingsHotKeys:
        {
            CSettingKeyboardHotkeyList hotkeys = getIContextSettings()->getHotkeys();
            CInputManager::getInstance()->changeHotkeySettings(hotkeys);
            break;
        }
        default:
            break;
        }
    }

    /*
     *  Dispatch message
     */
    void IContextApplication::messageHandlerDispatch(QtMsgType type, const QMessageLogContext &messageContext, const QString &message)
    {
        if (IContextApplication::s_oldHandler) IContextApplication::s_oldHandler(type, messageContext, message);
        if (IContextApplication::s_contexts.isEmpty()) return;
        foreach(IContextApplication *ctx, IContextApplication::s_contexts)
        {
            ctx->messageHandler(type, messageContext, message);
        }
    }

    /*
     * Handle message
     */
    void IContextApplication::messageHandler(QtMsgType type, const QMessageLogContext &messageContext, const QString &message)
    {
        Q_UNUSED(messageContext);
        RedirectionLevel outputRedirectionLevel = this->getOutputRedirectionLevel(); // local copy, thready safety
        if (outputRedirectionLevel == RedirectNone) return;
        CStatusMessage m(CStatusMessage::TypeStdoutRedirect, CStatusMessage::SeverityInfo, message);
        switch (type)
        {
        case QtDebugMsg:
            if (outputRedirectionLevel != RedirectAllOutput) return;
            break;
        case QtWarningMsg:
            if (outputRedirectionLevel == RedirectAllOutput) return;
            if (outputRedirectionLevel == RedirectError) return;
            m.setSeverity(CStatusMessage::SeverityWarning);
            break;
        case QtCriticalMsg:
            if (outputRedirectionLevel != RedirectError) return;
            m.setSeverity(CStatusMessage::SeverityError);
            break;
        case QtFatalMsg:
            if (m_outputRedirectionLevel != RedirectError) return;
            m.setSeverity(CStatusMessage::SeverityError);
            break;
        default:
            break;
        }

        // emit signal, based on thread id
        if (QThread::currentThread() == this->thread())
        {
            // same thread, can emit directly
            emit this->redirectedOutput(m, this->getUniqueId());
        }
        else
        {
            // Different threads, use invoke so that is called in "main / object's thread"
            // Otherwise for DBus: QtDBus: cannot relay signals from parent BlackCore::CContextApplication(0x4b4358 "") unless they are emitted in the object's thread QThread(0x4740b0 ""). Current thread is QThread(0x4b5530 "Thread (pooled)")
            QMetaObject::invokeMethod(this, "redirectedOutput", Q_ARG(BlackMisc::CStatusMessage, m), Q_ARG(qint64, this->getUniqueId()));
        }
    }

    /*
     * Redirected output
     */
    void IContextApplication::streamRedirectedOutput(const CStatusMessage &message, qint64 contextId)
    {
        if (this->getUniqueId() == contextId) return; // avoid infinite output
        RedirectionLevel redirectedOutputRedirectionLevel = this->getStreamingForRedirectedOutputLevel(); // local copy

        if (message.isEmpty()) return;
        switch (message.getSeverity())
        {
        case CStatusMessage::SeverityInfo:
            if (redirectedOutputRedirectionLevel != RedirectAllOutput) return;
            qDebug() << message.getMessage();
            break;
        case CStatusMessage::SeverityWarning:
            if (redirectedOutputRedirectionLevel == RedirectAllOutput) return;
            if (redirectedOutputRedirectionLevel == RedirectError) return;
            qWarning() << message.getMessage();
            break;
        case CStatusMessage::SeverityError:
            if (redirectedOutputRedirectionLevel != RedirectError) return;
            qCritical() << message.getMessage();
            break;
        }
    }
} // namespace
