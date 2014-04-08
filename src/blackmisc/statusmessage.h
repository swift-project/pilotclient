#ifndef BLACKMISC_STATUSMESSAGE_H
#define BLACKMISC_STATUSMESSAGE_H

#include "valueobject.h"
#include <QDateTime>

namespace BlackMisc
{

    /*!
     * \brief Streamable status message, e.g. from Core -> GUI
     */
    class CStatusMessage : public CValueObject
    {
    public:
        /*!
         * \brief Status types
         */
        enum StatusType
        {
            TypeUnknown, // not set
            TypeUnspecific, // intentionally set
            TypeValidation,
            TypeTrafficNetwork,
            TypeSimulator,
            TypeSettings,
            TypeCore,
            TypeAudio,
            TypeGui,
            TypeStdoutRedirect
        };

        //! Status severities
        enum StatusSeverity
        {
            SeverityInfo,
            SeverityWarning,
            SeverityError
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexType,
            IndexTypeAsString,
            IndexSeverity,
            IndexSeverityAsString,
            IndexMessage,
            IndexTimestamp
        };

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CStatusMessage)
        StatusType m_type;
        StatusSeverity m_severity;
        QString m_message;
        QDateTime m_timestamp;

    public:
        //! Constructor
        CStatusMessage() : m_type(TypeUnknown), m_severity(SeverityInfo) {}

        //! Constructor
        CStatusMessage(StatusType type, StatusSeverity severity, const char *message);

        //! Constructor
        CStatusMessage(StatusType type, StatusSeverity severity, const QString &message);

        //! Status type
        StatusType getType() const { return this->m_type; }

        //! Status severity
        StatusSeverity getSeverity() const { return this->m_severity; }

        //! Message
        QString getMessage() const { return this->m_message; }

        //! Message empty
        bool isEmpty() const { return this->m_message.isEmpty(); }

        //! \copydoc CValueObject::getValueHash()
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toQVariant()
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! Type as string
        const QString &getTypeAsString() const;

        //! Severity
        void setSeverity(StatusSeverity severity) { this->m_severity = severity; }

        //! Representing icon
        virtual const QPixmap &toIcon() const override { return CStatusMessage::convertToIcon(*this); }

        //! Type as string
        const QString &getSeverityAsString() const;

        //! \copydoc CValueObject::propertyByIndex(int)
        virtual QVariant propertyByIndex(int index) const override;

        //! \copydoc CValueObject::propertyByIndex(const QVariant, int)
        virtual void setPropertyByIndex(const QVariant &variant, int index) override;

        //! \copydoc CValueObject::propertyByIndexAsString
        virtual QString propertyByIndexAsString(int index, bool i18n) const override;

        //! To HTML
        QString toHtml() const;

        //! Equal operator ==
        bool operator ==(const CStatusMessage &other) const;

        //! Unequal operator !=
        bool operator !=(const CStatusMessage &other) const;

        //! Register metadata
        static void registerMetadata();

        //! Validation error
        static CStatusMessage getValidationError(const QString &message);

        //! (Unspecific) Info message
        static CStatusMessage getInfoMessage(const QString &message, StatusType type = CStatusMessage::TypeUnspecific);

        //! Representing icon
        static const QPixmap &convertToIcon(const CStatusMessage &statusMessage);

    protected:
        //! \copydoc CValueObject::marshallToDbus
        virtual void marshallToDbus(QDBusArgument &arg) const override;

        //! \copydoc CValueObject::unmarshallFromDbus
        virtual void unmarshallFromDbus(const QDBusArgument &arg) override;

        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

        //! \copydoc CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override;

        //! \copydoc CValueObject::isA
        virtual bool isA(int metaTypeId) const override;

        //! \copydoc CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &other) const override;
    };
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CStatusMessage, (o.m_type, o.m_severity, o.m_message, o.m_timestamp))
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)

#endif // guard
