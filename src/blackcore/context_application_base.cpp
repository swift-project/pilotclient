#include "blackcore/context_application_base.h"
#include "blackmisc/statusmessage.h"
#include <QCoreApplication>
#include <QThread>


using namespace BlackMisc;

namespace BlackCore
{

    QList<CContextApplicationBase *> CContextApplicationBase::s_contexts;
    QtMessageHandler CContextApplicationBase::s_oldHandler = nullptr;

    /*
     * Constructor
     */
    CContextApplicationBase::CContextApplicationBase(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextApplication(mode, runtime), m_outputRedirectionLevel(IContextApplication::RedirectNone), m_redirectedOutputRedirectionLevel(IContextApplication::RedirectNone)
    {
        if (CContextApplicationBase::s_contexts.isEmpty())
            CContextApplicationBase::s_oldHandler = qInstallMessageHandler(CContextApplicationBase::messageHandlerDispatch);
        CContextApplicationBase::s_contexts.append(this);
    }

    /*
     * Output data from redirect signal
     */
    void CContextApplicationBase::setStreamingForRedirectedOutputLevel(RedirectionLevel redirectionLevel)
    {
        disconnect(this, &IContextApplication::redirectedOutput, this, &CContextApplicationBase::streamRedirectedOutput);
        if (redirectionLevel != RedirectNone)
            connect(this, &IContextApplication::redirectedOutput, this, &CContextApplicationBase::streamRedirectedOutput);
        this->m_redirectedOutputRedirectionLevel = redirectionLevel;
    }

    /*
     * Process event in object's thread, used to emit signal from  other thread
     */
    bool CContextApplicationBase::event(QEvent *event)
    {
        if (event->type() == CApplicationEvent::eventType())
        {
            CApplicationEvent *e = static_cast<CApplicationEvent *>(event);
            emit this->redirectedOutput(e->m_message, this->getUniqueId());
            return true;
        }
        return IContextApplication::event(event);
    }

    /*
     * Reset output redirection
     */
    void CContextApplicationBase::resetOutputRedirection()
    {
        qInstallMessageHandler(0);
    }

    /*
     *  Dispatch message
     */
    void CContextApplicationBase::messageHandlerDispatch(QtMsgType type, const QMessageLogContext &messageContext, const QString &message)
    {
        if (CContextApplicationBase::s_oldHandler) CContextApplicationBase::s_oldHandler(type, messageContext, message);
        if (CContextApplicationBase::s_contexts.isEmpty()) return;
        CContextApplicationBase *ctx;
        foreach(ctx, CContextApplicationBase::s_contexts)
        {
            ctx->messageHandler(type, messageContext, message);
        }
    }

    /*
     * Handle message
     */
    void CContextApplicationBase::messageHandler(QtMsgType type, const QMessageLogContext &messageContext, const QString &message)
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
    void CContextApplicationBase::streamRedirectedOutput(const CStatusMessage &message, qint64 contextId)
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
