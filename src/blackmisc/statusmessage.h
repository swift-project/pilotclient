/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STATUSMESSAGE_H
#define BLACKMISC_STATUSMESSAGE_H

#include "icon.h"
#include "propertyindex.h"
#include <QDateTime>

namespace BlackMisc
{

    /*!
     * Streamable status message, e.g. from Core -> GUI
     */
    class CStatusMessage : public CValueObject
    {
    public:
        //! Status types
        enum StatusType
        {
            TypeUnknown,    //!< not set
            TypeUnspecific, //!< intentionally set, but not specific
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
            IndexType = BlackMisc::CPropertyIndex::GlobalIndexCStatusMessage,
            IndexTypeAsString,
            IndexSeverity,
            IndexSeverityAsString,
            IndexMessage,
            IndexTimestamp,
            IndexTimestampFormatted
        };

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

        //! \copydoc CValueObject::fromQVariant
        virtual void fromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! Type as string
        const QString &getTypeAsString() const;

        //! Severity
        void setSeverity(StatusSeverity severity) { this->m_severity = severity; }

        //! Representing icon
        virtual CIcon toIcon() const override { return CStatusMessage::convertToIcon(*this); }

        //! Type as string
        const QString &getSeverityAsString() const;

        //! \copydoc CValueObject::propertyByIndex(int)
        virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

        //! \copydoc CValueObject::propertyByIndex(const QVariant, int)
        virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

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

        //! (Unspecific) Warning message
        static CStatusMessage getWarningMessage(const QString &message, StatusType type = CStatusMessage::TypeUnspecific);

        //! (Unspecific) Error message
        static CStatusMessage getErrorMessage(const QString &message, StatusType type = CStatusMessage::TypeUnspecific);

        //! Representing icon
        static const CIcon &convertToIcon(const CStatusMessage &statusMessage);

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

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CStatusMessage)
        StatusType m_type;
        StatusSeverity m_severity;
        QString m_message;
        QDateTime m_timestamp;

    };
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CStatusMessage, (o.m_type, o.m_severity, o.m_message, o.m_timestamp))
Q_DECLARE_METATYPE(BlackMisc::CStatusMessage)

#endif // guard
