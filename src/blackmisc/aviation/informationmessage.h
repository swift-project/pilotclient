/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_INFORMATIONMESSAGE_H
#define BLACKMISC_AVIATION_INFORMATIONMESSAGE_H

#include "blackmisc/timestampbased.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/dbus.h"
#include "blackmisc/blackmiscexport.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QtGlobal>

namespace BlackMisc::Aviation
{
    //! Value object encapsulating information message (ATIS, METAR, TAF)
    class BLACKMISC_EXPORT CInformationMessage :
        public CValueObject<CInformationMessage>,
        public ITimestampBased
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
        CInformationMessage() : m_type(CInformationMessage::Unspecified)
        {}

        //! Information message of type
        explicit CInformationMessage(InformationType type) : m_type(type) {}

        //! Information message of type
        CInformationMessage(InformationType type, const QString &message)
            : m_type(type), m_message(message)
        {}

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
        qint64 timeDiffReceivedMs() const
        {
            return this->getTimeDifferenceToNowMs();
        }

        //! Is empty?
        bool isEmpty() const { return m_message.isEmpty(); }

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CInformationMessage &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::CValueObject::registerMetadata
        static void registerMetadata();

        //! Unspecified object
        static const CInformationMessage &unspecified();

    private:
        InformationType m_type;
        QString m_message;

        BLACK_METACLASS(
            CInformationMessage,
            BLACK_METAMEMBER(type),
            BLACK_METAMEMBER(message),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CInformationMessage)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CInformationMessage::InformationType)

#endif // guard
