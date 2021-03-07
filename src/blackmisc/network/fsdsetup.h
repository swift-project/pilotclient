/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_FSDSETUP_H
#define BLACKMISC_NETWORK_FSDSETUP_H

#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"

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
                IndexTextCodec = CPropertyIndexRef::GlobalIndexCFsdSetup,
                IndexSendReceiveDetails
            };

            //! Send/receive details
            enum SendReceiveDetailsFlag
            {
                Nothing                 = 0,      //!< nothing
                SendAircraftParts       = 1 << 0, //!< aircraft parts out
                SendInterimPositions    = 1 << 1, //!< interim positions out
                SendGndFlag             = 1 << 2, //!< gnd.flag out (position)
                ReceiveAircraftParts    = 1 << 3, //!< aircraft parts in
                ReceiveInterimPositions = 1 << 4, //!< fast position updates in
                ReceiveGndFlag          = 1 << 5, //!< gnd.flag in (position)
                Force3LetterAirlineICAO = 1 << 6, //!< force 3 letter airline ICAO code
                SendVisualPositions     = 1 << 7, //!< visual positions out
                AllSending              = SendAircraftParts | SendInterimPositions | SendVisualPositions | SendGndFlag, //!< all out
                AllReceive              = ReceiveAircraftParts | ReceiveInterimPositions | ReceiveGndFlag, //!< all in
                All                     = AllReceive | AllSending, //!< all
                AllParts                = SendAircraftParts | ReceiveAircraftParts,       //!< send/receive parts
                AllSendingWithoutGnd    = AllSending - SendGndFlag,                       //!< all out, but no gnd.flag
                AllReceiveWithoutGnd    = AllReceive - ReceiveGndFlag,                    //!< all in, but no gnd.flag
                AllInterimPositions     = SendInterimPositions | ReceiveInterimPositions, //!< all interim positions
                AllWithoutGnd           = AllReceiveWithoutGnd | AllSendingWithoutGnd,    //!< all, but no gnd.flag
                VATSIMDefault           = AllParts | Force3LetterAirlineICAO | SendVisualPositions
            };
            Q_DECLARE_FLAGS(SendReceiveDetails, SendReceiveDetailsFlag)

            //! Offset times basically telling when to expect the next value from network plus some reserve
            //! \remark related to CNetworkVatlib::c_updatePostionIntervalMsec / c_updateInterimPostionIntervalMsec
            //! @{
            static constexpr qint64 c_positionTimeOffsetMsec = 6000;        //!< offset time for received position updates Ref T297
            static constexpr qint64 c_minimumPositionTimeOffsetMsec = 700;  //!< offset time for vatsim high frequency position updates
            //! @}

            //! Default constructor.
            CFsdSetup() {}

            //! Constructor.
            CFsdSetup(SendReceiveDetails sendReceive);

            //! Constructor.
            CFsdSetup(const QString &codec, SendReceiveDetails sendReceive = All);

            //! Get codec
            const QString &getTextCodec() const { return m_textCodec; }

            //! Set codec
            void setTextCodec(const QString &codec) { m_textCodec = codec.trimmed(); }

            //! Get send / receive details
            SendReceiveDetails getSendReceiveDetails() const;

            //! Set send / receive details
            void setSendReceiveDetails(SendReceiveDetails sendReceive) { m_sendReceive = sendReceive; }

            //! Add send / receive details
            void addSendReceiveDetails(SendReceiveDetails sendReceive) { m_sendReceive |= sendReceive; }

            //! Remove send / receive details
            void removeSendReceiveDetails(SendReceiveDetails sendReceive) { m_sendReceive &= ~sendReceive; }

            //! Set send / receive details
            void setSendReceiveDetails(bool partsSend, bool partsReceive, bool gndSend, bool gndReceive, bool interimSend, bool interimReceive, bool visualSend);

            //! FSD setup flags
            //! @{
            bool sendAircraftParts() const { return this->getSendReceiveDetails().testFlag(SendAircraftParts); }
            bool sendGndFlag() const { return this->getSendReceiveDetails().testFlag(SendGndFlag); }
            bool sendInterimPositions() const { return this->getSendReceiveDetails().testFlag(SendInterimPositions); }
            bool sendVisualPositions() const { return this->getSendReceiveDetails().testFlag(SendVisualPositions); }

            bool receiveAircraftParts() const { return this->getSendReceiveDetails().testFlag(ReceiveAircraftParts); }
            bool receiveGndFlag() const { return this->getSendReceiveDetails().testFlag(ReceiveGndFlag); }
            bool receiveInterimPositions() const { return this->getSendReceiveDetails().testFlag(ReceiveInterimPositions); }
            //! @}

            //! Airline codes
            //! @{
            bool force3LetterAirlineCodes() const { return this->getSendReceiveDetails().testFlag(Force3LetterAirlineICAO); }
            void setForce3LetterAirlineCodes(bool force);
            //! @}

            //! Validate, provide details about issues
            CStatusMessageList validate() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CFsdSetup &compareValue) const;

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
