/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_FSDSETUP_H
#define BLACKMISC_NETWORK_FSDSETUP_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object for a FSD setup
        class BLACKMISC_EXPORT CFsdSetup : public CValueObject<CFsdSetup>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexTextCodec = BlackMisc::CPropertyIndex::GlobalIndexCFsdSetup,
                IndexSendReceiveDetails
            };

            //! Send/receive details
            enum SendReceiveDetailsFlag
            {
                Nothing                 = 0,         //!< nothing
                SendAircraftParts       = 1 << 0,    //!< aircraft parts out
                SendIterimPositions     = 1 << 1,    //!< interim positions in
                ReceiveAircraftParts    = 1 << 2,    //!< fast position updates out
                ReceiveInterimPositions = 1 << 3,    //!< fast position updates in
                AllSending              = SendAircraftParts | SendIterimPositions,        //!< all out
                AllReceive              = ReceiveAircraftParts | ReceiveInterimPositions, //!< all in
                All                     = AllReceive | AllSending                         //!< all
            };
            Q_DECLARE_FLAGS(SendReceiveDetails, SendReceiveDetailsFlag)

            //! Default constructor.
            CFsdSetup() {}

            //! Constructor.
            CFsdSetup(const QString &codec, SendReceiveDetails sendReceive = All);

            //! Get codec
            const QString &getTextCodec() const { return m_textCodec; }

            //! Set codec
            void setTextCodec(const QString &codec) { m_textCodec = codec.trimmed().toLower(); }

            //! Get send / receive details
            SendReceiveDetails getSendReceiveDetails() const;

            //! Set send / receive details
            void setSendReceiveDetails(SendReceiveDetails sendReceive) { m_sendReceive = sendReceive; }

            //! Set send / receive details
            void setSendReceiveDetails(bool partsSend, bool partsReceive, bool interimSend, bool interimReceive);

            //! Validate, provide details about issues
            BlackMisc::CStatusMessageList validate() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! Details as string
            static QString sendReceiveDetailsToString(SendReceiveDetails details);

            //! Standard FSD setup for official VATSIM servers
            static const CFsdSetup &vatsimStandard();

        private:
            QString m_textCodec = "latin1";
            int m_sendReceive = static_cast<int>(All);

            BLACK_METACLASS(
                CFsdSetup,
                BLACK_METAMEMBER(textCodec),
                BLACK_METAMEMBER(sendReceive)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CFsdSetup)
Q_DECLARE_METATYPE(BlackMisc::Network::CFsdSetup::SendReceiveDetails)
Q_DECLARE_METATYPE(BlackMisc::Network::CFsdSetup::SendReceiveDetailsFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Network::CFsdSetup::SendReceiveDetails)

#endif // guard
