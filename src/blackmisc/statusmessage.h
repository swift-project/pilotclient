#ifndef BLACKMISC_STATUSMESSAGE_H
#define BLACKMISC_STATUSMESSAGE_H

#include "blackmiscfreefunctions.h"
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
            TypeSettings,
            TypeCore,
            TypeAudio,
            TypeGui
        };

        /*!
         * \brief Status severities
         */
        enum StatusSeverity
        {
            SeverityInfo,
            SeverityWarning,
            SeverityError
        };

        /*!
         * \brief Properties by index
         */
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
        //! \brief Constructor
        CStatusMessage() : m_type(TypeUnknown), m_severity(SeverityInfo) {}

        //! \brief Constructor
        CStatusMessage(StatusType type, StatusSeverity severity, const QString &message);

        //! \brief Status type
        StatusType getType() const
        {
            return this->m_type;
        }

        //! \brief Status severity
        StatusSeverity getSeverity() const
        {
            return this->m_severity;
        }

        //! \brief Message
        QString getMessage() const
        {
            return this->m_message;
        }

        //! \copydoc CValueObject::getValueHash()
        virtual uint getValueHash() const override;

        //! \copydoc CValueObject::toQVariant()
        virtual QVariant toQVariant() const override
        {
            return QVariant::fromValue(*this);
        }

        //! \brief Type as string
        const QString &getTypeAsString() const;

        //! \brief representing icon
        virtual const QPixmap &toIcon() const override { return CStatusMessage::convertToIcon(*this); }

        //! \brief Type as string
        const QString &getSeverityAsString() const;

        //! \copydoc CValueObject::propertyByIndex(int)
        virtual QVariant propertyByIndex(int index) const override;

        //! \copydoc CValueObject::propertyByIndex(const QVariant, int)
        virtual void setPropertyByIndex(const QVariant &variant, int index) override;

        //! \copydoc CValueObject::propertyByIndexAsString
        virtual QString propertyByIndexAsString(int index, bool i18n) const override;

        //! \brief Equal operator ==
        bool operator ==(const CStatusMessage &other) const;

        //! \brief Unequal operator !=
        bool operator !=(const CStatusMessage &other) const;

        //! \brief Register metadata
        static void registerMetadata();

        //! \brief Validation error
        static CStatusMessage getValidationError(const QString &message);

        //! \brief (Unspecific) Info message
        static CStatusMessage getInfoMessage(const QString &message, StatusType type = CStatusMessage::TypeUnspecific);

        //! \brief representing icon
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

BLACK_DBUS_ENUM_MARSHALLING(BlackMisc::CStatusMessage::StatusSeverity)
BLACK_DBUS_ENUM_MARSHALLING(BlackMisc::CStatusMessage::StatusType)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CStatusMessage, (o.m_type, o.m_severity, o.m_message, o.m_timestamp))
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)

#endif // guard
