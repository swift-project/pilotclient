#ifndef BLACKMISC_STATUSMESSAGE_H
#define BLACKMISC_STATUSMESSAGE_H

#include "valueobject.h"

namespace BlackMisc
{

    /*!
     * \brief Streamable status message, e.g. from Core -> GUI
     */
    class CStatusMessage : public CValueObject
    {
    public:
        /*!
         * \brief The StatusType enum
         */
        enum StatusType
        {
            TypeUnknown, // not set
            TypeUnspecific, // intentionally set
            TypeValidation,
            TypeTrafficNetwork,
            TypeSettings,
            TypeCore
        };

        enum StatusSeverity
        {
            SeverityInfo,
            SeverityWarning,
            SeverityError
        };

    private:
        StatusType m_type;
        StatusSeverity m_severity;
        QString m_message;
        QDateTime m_timestamp;

    public:
        /*!
         * \brief StatusMessage
         */
        CStatusMessage() :
            m_type(TypeUnknown), m_severity(SeverityInfo) {}

        /*!
         * \brief StatusMessage
         * \param type
         * \param severity
         * \param message
         */
        CStatusMessage(StatusType type, StatusSeverity severity, const QString &message);

        /*!
         * \brief Status type
         * \return
         */
        StatusType getType() const
        {
            return this->m_type;
        }

        /*!
         * \brief Status severity
         * \return
         */
        StatusSeverity getSeverity() const
        {
            return this->m_severity;
        }

        /*!
         * \brief Message
         * \return
         */
        QString getMessage() const
        {
            return this->m_message;
        }

        /*!
         * \brief Value hash
         * \return
         */
        virtual uint getValueHash() const;

        /*!
         * \brief As QVariant
         * \return
         */
        QVariant asQVariant() const
        {
            return QVariant::fromValue(*this);
        }

        /*!
         * \brief Register metadata
         */
        static void registerMetadata();

        /*!
         * \brief Validation error
         * \param message
         * \return
         */
        static CStatusMessage getValidationError(const QString &message);

        /*!
         * \brief Unspecific info message
         * \param message
         * \return
         */
        static CStatusMessage getInfoMessage(const QString &message);


    protected:
        /*!
         * \brief Marshall to DBus
         * \param argument
         */
        virtual void marshallToDbus(QDBusArgument &arg) const;

        /*!
         * \brief Unmarshall from DBus
         * \param argument
         */
        virtual void unmarshallFromDbus(const QDBusArgument &arg);

        /*!
         * \brief Convert to String
         * \param i18n
         * \return
         */
        virtual QString convertToQString(bool i18n = false) const;
    };

}

Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)

#endif // guard
