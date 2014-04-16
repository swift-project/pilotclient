#include "blackcore/context_application.h"
#include "blackcore/context_application_event.h"
#include "blackmisc/statusmessage.h"
#include <QCoreApplication>
#include <QThread>

using namespace BlackMisc;

namespace BlackCore
{

    QList<IContextApplication *> IContextApplication::s_contexts;
    QtMessageHandler IContextApplication::s_oldHandler = nullptr;

    /*
     * Constructor
     */
    IContextApplication::IContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        CContext(mode, runtime), m_outputRedirectionLevel(IContextApplication::RedirectNone), m_redirectedOutputRedirectionLevel(IContextApplication::RedirectNone)
    {
        if (IContextApplication::s_contexts.isEmpty())
            IContextApplication::s_oldHandler = qInstallMessageHandler(IContextApplication::messageHandlerDispatch);
        IContextApplication::s_contexts.append(this);
    }

    /*
     * Output data from redirect signal
     */
    void IContextApplication::setStreamingForRedirectedOutputLevel(RedirectionLevel redirectionLevel)
    {
        disconnect(this, &IContextApplication::redirectedOutput, this, &IContextApplication::streamRedirectedOutput);
        if (redirectionLevel != RedirectNone)
            connect(this, &IContextApplication::redirectedOutput, this, &IContextApplication::streamRedirectedOutput);
        this->m_redirectedOutputRedirectionLevel = redirectionLevel;
    }

    /*
     * Process event in object's thread, used to emit signal from  other thread
     */
    bool IContextApplication::event(QEvent *event)
    {
        if (event->type() == CApplicationEvent::eventType())
        {
            CApplicationEvent *e = static_cast<CApplicationEvent *>(event);
            emit this->redirectedOutput(e->m_message, this->getUniqueId());
            return true;
        }
        return CContext::event(event);
    }

    /*
     * Reset output redirection
     */
    void IContextApplication::resetOutputRedirection()
    {
        qInstallMessageHandler(0);
    }

    /*
     *  Dispatch message
     */
    void IContextApplication::messageHandlerDispatch(QtMsgType type, const QMessageLogContext &messageContext, const QString &message)
    {
        if (IContextApplication::s_oldHandler) IContextApplication::s_oldHandler(type, messageContext, message);
        if (IContextApplication::s_contexts.isEmpty()) return;
        IContextApplication *ctx;
        foreach(ctx, IContextApplication::s_contexts)
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
        if (this->m_outputRedirectionLevel == RedirectNone) return;
        CStatusMessage m(CStatusMessage::TypeStdoutRedirect, CStatusMessage::SeverityInfo, message);
        switch (type)
        {
        case QtDebugMsg:
            if (this->m_outputRedirectionLevel != RedirectAllOutput) return;
            break;
        case QtWarningMsg:
            if (this->m_outputRedirectionLevel == RedirectAllOutput) return;
            if (this->m_outputRedirectionLevel == RedirectError) return;
            m.setSeverity(CStatusMessage::SeverityWarning);
            break;
        case QtCriticalMsg:
            if (this->m_outputRedirectionLevel != RedirectError) return;
            m.setSeverity(CStatusMessage::SeverityError);
            break;
        case QtFatalMsg:
            if (this->m_outputRedirectionLevel != RedirectError) return;
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
            // different threads, use event.
            // in this event the same redirect as above will emiited, only that this is then
            // done in the same thread as the parent object
            CApplicationEvent *e = new CApplicationEvent(m, this->getUniqueId());
            QCoreApplication::postEvent(this, e);
        }
    }

    /*
     * Redirected output
     */
    void IContextApplication::streamRedirectedOutput(const CStatusMessage &message, qint64 contextId)
    {
        if (this->getUniqueId() == contextId) return; // avoid infinite output
        if (this->m_redirectedOutputRedirectionLevel == RedirectNone) return;

        if (message.isEmpty()) return;
        switch (message.getSeverity())
        {
        case CStatusMessage::SeverityInfo:
            if (this->m_redirectedOutputRedirectionLevel != RedirectAllOutput) return;
            qDebug() << message.getMessage();
            break;
        case CStatusMessage::SeverityWarning:
            if (this->m_redirectedOutputRedirectionLevel == RedirectAllOutput) return;
            if (this->m_redirectedOutputRedirectionLevel == RedirectError) return;
            qWarning() << message.getMessage();
            break;
        case CStatusMessage::SeverityError:
            if (this->m_redirectedOutputRedirectionLevel != RedirectError) return;
            qCritical() << message.getMessage();
            break;
        }
    }
} // namespace
