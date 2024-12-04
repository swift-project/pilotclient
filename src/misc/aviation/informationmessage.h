// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_INFORMATIONMESSAGE_H
#define SWIFT_MISC_AVIATION_INFORMATIONMESSAGE_H

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QtGlobal>

#include "misc/dbus.h"
#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CInformationMessage)

namespace swift::misc::aviation
{
    //! Value object encapsulating information message (ATIS, METAR, TAF)
    class SWIFT_MISC_EXPORT CInformationMessage : public CValueObject<CInformationMessage>, public ITimestampBased
    {
    public:
        //! Type
        enum InformationType
        {
            Unspecified,
            ATIS,
            METAR,
            TAF
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexType = CPropertyIndexRef::GlobalIndexCInformationMessage,
            IndexMessage,
        };

        //! Default constructor.
        CInformationMessage() : m_type(CInformationMessage::Unspecified) {}

        //! Information message of type
        explicit CInformationMessage(InformationType type) : m_type(type) {}

        //! Information message of type
        CInformationMessage(InformationType type, const QString &message) : m_type(type), m_message(message) {}

        //! Get message.
        const QString &getMessage() const { return m_message; }

        //! Is a message available
        bool hasMessage() const { return !m_message.isEmpty(); }

        //! Set message
        void setMessage(const QString &message)
        {
            this->setCurrentUtcTime();
            m_message = message;
        }

        //! Append message part
        void appendMessage(const QString &messagePart)
        {
            this->setCurrentUtcTime();
            m_message.append(messagePart);
        }

        //! Type as string
        const QString &getTypeAsString() const;

        //! Type
        InformationType getType() const { return m_type; }

        //! Set type
        void setType(InformationType type) { m_type = type; }

        //! Received before n ms
        qint64 timeDiffReceivedMs() const { return this->getTimeDifferenceToNowMs(); }

        //! Is empty?
        bool isEmpty() const { return m_message.isEmpty(); }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CInformationMessage &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::CValueObject::registerMetadata
        static void registerMetadata();

        //! Unspecified object
        static const CInformationMessage &unspecified();

    private:
        InformationType m_type;
        QString m_message;

        SWIFT_METACLASS(
            CInformationMessage,
            SWIFT_METAMEMBER(type),
            SWIFT_METAMEMBER(message),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CInformationMessage)
Q_DECLARE_METATYPE(swift::misc::aviation::CInformationMessage::InformationType)

#endif // SWIFT_MISC_AVIATION_INFORMATIONMESSAGE_H
