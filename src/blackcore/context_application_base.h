#ifndef BLACKCORE_CONTEXT_APPLICATION_BASE_H
#define BLACKCORE_CONTEXT_APPLICATION_BASE_H

#include "blackcore/context_application.h"
#include <QEvent>

namespace BlackCore
{
    /*!
     * \brief Class, implementing streaming handling for application context
     */
    class CContextApplicationBase : public IContextApplication
    {

    public:

        //! Destructor
        virtual ~CContextApplicationBase() {}

        //! \copydoc IContextApplication::getOutputRedirectionLevel
        virtual RedirectionLevel getOutputRedirectionLevel() const override { return this->m_outputRedirectionLevel; }

        //! \copydoc IContextApplication::setOutputRedirectionLevel
        virtual void setOutputRedirectionLevel(RedirectionLevel redirectionLevel) override { this->m_outputRedirectionLevel = redirectionLevel; }

        //! \copydoc IContextApplication::getStreamingForRedirectedOutputLevel
        virtual RedirectionLevel getStreamingForRedirectedOutputLevel() const override { return this->m_redirectedOutputRedirectionLevel; }

        //! \copydoc IContextApplication::setStreamingForRedirectedOutputLevel
        virtual void setStreamingForRedirectedOutputLevel(RedirectionLevel redirectionLevel) override;

        //! Process event, cross thread messages
        virtual bool event(QEvent *event) override;

        //! Reset output redirection
        static void resetOutputRedirection();

    protected:
        //! Constructor
        CContextApplicationBase(CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    private:
        //! All contexts, used with messageHandler
        static QList<CContextApplicationBase *> s_contexts;

        //! Previous message handler
        static QtMessageHandler s_oldHandler;

        //! Message handler, handles one individual context
        void messageHandler(QtMsgType type, const QMessageLogContext &messageContext, const QString &messsage);

        //! Handle output dispatch, handles all contexts
        static void messageHandlerDispatch(QtMsgType type, const QMessageLogContext &messageContext, const QString &message);

        RedirectionLevel m_outputRedirectionLevel; //!< enable / disable my output
        RedirectionLevel m_redirectedOutputRedirectionLevel; //!< enable / disable others output

    private slots:
        //! Re-stream the redirected output
        void streamRedirectedOutput(const BlackMisc::CStatusMessage &message, qint64 contextId);
    };

    /*!
     * \brief Event to allow cross thread output redirection
     */
    class CApplicationEvent : public QEvent
    {
        friend class CContextApplicationBase;

    public:
        //! Constructor
        CApplicationEvent(const BlackMisc::CStatusMessage &msg, qint64 contextId) :
            QEvent(eventType()), m_message(msg), m_contextId(contextId) {}
        //! Destructor
        virtual ~CApplicationEvent() {}
        //! Event type
        static const QEvent::Type &eventType()
        {
            const static QEvent::Type t = static_cast<QEvent::Type>(QEvent::registerEventType());
            return t;
        }

    private:
        BlackMisc::CStatusMessage m_message;
        qint64 m_contextId;
    };

} // namespace

#endif // guard
