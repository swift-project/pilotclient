// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_RAWFSDMESSAGE_H
#define BLACKMISC_NETWORK_RAWFSDMESSAGE_H

#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CRawFsdMessage)

namespace BlackMisc::Network
{
    //! Value object for a raw FSD message
    class BLACKMISC_EXPORT CRawFsdMessage :
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

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_rawMessage;

        BLACK_METACLASS(
            CRawFsdMessage,
            BLACK_METAMEMBER(rawMessage),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CRawFsdMessage)

#endif // guard
