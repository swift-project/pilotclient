// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_FSDSETUP_H
#define SWIFT_MISC_NETWORK_FSDSETUP_H

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CFsdSetup)

namespace swift::misc::network
{
    //! Value object for a FSD setup
    class SWIFT_MISC_EXPORT CFsdSetup : public CValueObject<CFsdSetup>
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
            Nothing = 0, //!< nothing
            SendAircraftParts = 1 << 0, //!< aircraft parts out
            SendInterimPositions = 1 << 1, //!< interim positions out
            SendGndFlag = 1 << 2, //!< gnd.flag out (position)
            ReceiveAircraftParts = 1 << 3, //!< aircraft parts in
            ReceiveInterimPositions = 1 << 4, //!< fast position updates in
            ReceiveGndFlag = 1 << 5, //!< gnd.flag in (position)
            Force3LetterAirlineICAO = 1 << 6, //!< force 3 letter airline ICAO code
            SendVisualPositions = 1 << 7, //!< visual positions out
            ReceiveEuroscopeSimData = 1 << 8, //!< euroscope SIMDATA in
            SendFplWithIcaoEquipment = 1 << 9, //!< send flightplan with ICAO equipment code instead of FAA code
            AllSending = SendAircraftParts | SendInterimPositions | SendVisualPositions | SendGndFlag, //!< all out
            AllReceive = ReceiveAircraftParts | ReceiveInterimPositions | ReceiveGndFlag, //!< all in
            All = AllReceive | AllSending, //!< all
            AllParts = SendAircraftParts | ReceiveAircraftParts, //!< send/receive parts
            AllSendingWithoutGnd = AllSending - SendGndFlag, //!< all out, but no gnd.flag
            AllReceiveWithoutGnd = AllReceive - ReceiveGndFlag, //!< all in, but no gnd.flag
            AllInterimPositions = SendInterimPositions | ReceiveInterimPositions, //!< all interim positions
            AllWithoutGnd = AllReceiveWithoutGnd | AllSendingWithoutGnd, //!< all, but no gnd.flag
            VATSIMDefault = AllParts | Force3LetterAirlineICAO | SendVisualPositions | SendFplWithIcaoEquipment
        };
        Q_DECLARE_FLAGS(SendReceiveDetails, SendReceiveDetailsFlag)

        //! @{
        //! Offset times basically telling when to expect the next value from network plus some reserve
        //! \remark related to CNetworkVatlib::c_updatePostionIntervalMsec / c_updateInterimPostionIntervalMsec
        static constexpr qint64 c_positionTimeOffsetMsec = 6000; //!< offset time for received position updates Ref T297
        static constexpr qint64 c_interimPositionTimeOffsetMsec = 1500; //!< offset time for received interim position updates Ref T297
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
        void setSendReceiveDetails(bool partsSend, bool partsReceive, bool gndSend, bool gndReceive, bool interimSend, bool interimReceive, bool visualSend, bool euroscopeSimDataReceive, bool icaoEquipment);

        //! @{
        //! FSD setup flags
        bool sendAircraftParts() const { return this->getSendReceiveDetails().testFlag(SendAircraftParts); }
        bool sendGndFlag() const { return this->getSendReceiveDetails().testFlag(SendGndFlag); }
        bool sendInterimPositions() const { return this->getSendReceiveDetails().testFlag(SendInterimPositions); }
        bool sendVisualPositions() const { return this->getSendReceiveDetails().testFlag(SendVisualPositions); }

        bool receiveAircraftParts() const { return this->getSendReceiveDetails().testFlag(ReceiveAircraftParts); }
        bool receiveGndFlag() const { return this->getSendReceiveDetails().testFlag(ReceiveGndFlag); }
        bool receiveInterimPositions() const { return this->getSendReceiveDetails().testFlag(ReceiveInterimPositions); }
        bool receiveEuroscopeSimData() const { return this->getSendReceiveDetails().testFlag(ReceiveEuroscopeSimData); }
        bool shouldSendFlightPlanEquipmentInIcaoFormat() const { return this->getSendReceiveDetails().testFlag(SendFplWithIcaoEquipment); }
        //! @}

        //! @{
        //! Airline codes
        bool force3LetterAirlineCodes() const { return this->getSendReceiveDetails().testFlag(Force3LetterAirlineICAO); }
        void setForce3LetterAirlineCodes(bool force);
        //! @}

        //! Validate, provide details about issues
        CStatusMessageList validate() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CFsdSetup &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! Details as string
        static QString sendReceiveDetailsToString(SendReceiveDetails details);

        //! Standard FSD setup for official VATSIM servers
        static const CFsdSetup &vatsimStandard();

    private:
        QString m_textCodec = "latin1";
        int m_sendReceive = static_cast<int>(All);

        SWIFT_METACLASS(
            CFsdSetup,
            SWIFT_METAMEMBER(textCodec),
            SWIFT_METAMEMBER(sendReceive));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CFsdSetup)
Q_DECLARE_METATYPE(swift::misc::network::CFsdSetup::SendReceiveDetails)
Q_DECLARE_METATYPE(swift::misc::network::CFsdSetup::SendReceiveDetailsFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::network::CFsdSetup::SendReceiveDetails)

#endif // guard
