#ifndef BLACKCORE_CONTEXT_APPLICATION_EVENT_H
#define BLACKCORE_CONTEXT_APPLICATION_EVENT_H

#include "blackcore/context_application.h"
#include <QEvent>

namespace BlackCore
{
    /*!
     * \brief Event to allow cross thread output redirection
     */
    class CApplicationEvent : public QEvent
    {
        friend class IContextApplication;

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
