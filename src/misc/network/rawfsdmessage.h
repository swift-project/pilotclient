// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_RAWFSDMESSAGE_H
#define SWIFT_MISC_NETWORK_RAWFSDMESSAGE_H

#include <QDateTime>
#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CRawFsdMessage)

namespace swift::misc::network
{
    //! Value object for a raw FSD message
    class SWIFT_MISC_EXPORT CRawFsdMessage :
        public CValueObject<CRawFsdMessage>,
        public ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexRawMessage = CPropertyIndexRef::GlobalIndexCRawFsdMessage,
        };

        //! Default constructor.
        CRawFsdMessage() = default;

        //! Constructor.
        CRawFsdMessage(const QString &rawMessage);

        //! Get raw message
        const QString &getRawMessage() const { return m_rawMessage; }

        //! Set raw message
        void setRawMessage(const QString &rawMessage) { m_rawMessage = rawMessage; }

        //! Returns true if the raw message is from the given PDU packet type
        bool isPacketType(const QString &type) const;

        //! Does the raw message contain str?
        bool containsString(const QString &str) const;

        //! Returns a list of all known packet types.
        static const QStringList &getAllPacketTypes();

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_rawMessage;

        SWIFT_METACLASS(
            CRawFsdMessage,
            SWIFT_METAMEMBER(rawMessage),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CRawFsdMessage)

#endif // guard
