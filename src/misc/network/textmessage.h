// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_TEXTMESSAGE_H
#define SWIFT_MISC_NETWORK_TEXTMESSAGE_H

#include <QMetaType>
#include <QPixmap>
#include <QString>

#include "misc/aviation/callsign.h"
#include "misc/metaclass.h"
#include "misc/pq/frequency.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CTextMessage)

namespace swift::misc::network
{
    /*!
     * Value object encapsulating information of a text message
     */
    class SWIFT_MISC_EXPORT CTextMessage : public CValueObject<CTextMessage>, public ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexSenderCallsign = CPropertyIndexRef::GlobalIndexCTextMessage,
            IndexRecipientCallsign,
            IndexRecipientCallsignOrFrequency,
            IndexMessage
        };

        //! Default constructor.
        CTextMessage() {}

        //! Constructor, radio message
        CTextMessage(const QString &message, const physical_quantities::CFrequency &frequency,
                     const aviation::CCallsign &senderCallsign = {});

        //! Constructor, private message
        CTextMessage(const QString &message, const aviation::CCallsign &senderCallsign,
                     const aviation::CCallsign &recipientCallsign = {});

        //! Get callsign (from)
        const aviation::CCallsign &getSenderCallsign() const { return m_senderCallsign; }

        //! Set callsign (from)
        void setSenderCallsign(const aviation::CCallsign &callsign) { m_senderCallsign = callsign; }

        //! Get callsign (to)
        const aviation::CCallsign &getRecipientCallsign() const { return m_recipientCallsign; }

        //! Set callsign (recipient)
        void setRecipientCallsign(const aviation::CCallsign &callsign) { m_recipientCallsign = callsign; }

        //! Get recipient or frequency
        QString getRecipientCallsignOrFrequency() const;

        //! Send to particular frequency?
        bool isSendToFrequency(const physical_quantities::CFrequency &frequency) const;

        //! Send to UNICOM?
        bool isSendToUnicom() const;

        //! Valid receviver?
        bool hasValidRecipient() const;

        //! Get message
        const QString &getMessage() const { return m_message; }

        //! Is the callsign given mentioned in that message
        bool mentionsCallsign(const aviation::CCallsign &callsign) const;

        //! Get ASCII only message
        QString getAsciiOnlyMessage() const;

        //! Get HTML escaped message
        QString getHtmlEncodedMessage() const;

        //! Empty message
        bool isEmpty() const { return m_message.isEmpty(); }

        //! Set message
        void setMessage(const QString &message);

        //! Get frequency
        const physical_quantities::CFrequency &getFrequency() const { return m_frequency; }

        //! Set frequency
        void setFrequency(const physical_quantities::CFrequency &frequency) { m_frequency = frequency; }

        //! Is private message?
        bool isPrivateMessage() const;

        //! Is radio message?
        bool isRadioMessage() const;

        //! Initial message of server?
        bool isServerMessage() const;

        //! Is this a broadcast message
        bool isBroadcastMessage() const;

        //! Is this a message send via .wallop
        bool isWallopMessage() const;

        //! Whole message as formatted string. Used to display message in a console window.
        //! \param withSender        include sender information in string?
        //! \param withRecipient     include recipient information in string?
        //! \param separator         values separated by given value
        QString asString(bool withSender, bool withRecipient, const QString &separator = ", ") const;

        //! Whole message as swift::misc::CStatusMessage.
        //! Used to display message in logs, or any other situation where a CStatusMessage is required.
        //! \param withSender        include sender information in string?
        //! \param withRecipient     include recipient information in string?
        //! \param separator         values separated by given value
        CStatusMessage asStatusMessage(bool withSender, bool withRecipient, const QString &separator = ", ") const;

        //! Summary HTML code
        QString asHtmlSummary(const QString &separator = "<br>") const;

        //! Toggle sender receiver, can be used to ping my own message
        void toggleSenderRecipient();

        //! Is this a text message encapsulating a SELCAL
        //! \see http://forums.vatsim.net/viewtopic.php?f=8&t=63467
        bool isSelcalMessage() const;

        //! Is this a text message encapsulating a SELCAL for given code?
        bool isSelcalMessageFor(const QString &selcal) const;

        //! Supervisor message?
        bool isSupervisorMessage() const;

        //! Was sent?
        bool wasSent() const { return m_wasSent; }

        //! Mark as sent
        //! \remark also sets current timestamp if there is no valid timestamp
        void markAsSent();

        //! Is relayed message
        bool isRelayedMessage() const;

        //! Mark as relayed message
        void markAsRelayedMessage() { m_relayedMessage = true; }

        //! Mark as broadcast message
        void markAsBroadcastMessage();

        //! Mark as relayed and keep original sender
        void makeRelayedMessage(const aviation::CCallsign &partnerCallsign);

        //! Turn relayed message into private message
        bool relayedMessageToPrivateMessage();

        //! Can another message be appended
        bool canBeAppended(const CTextMessage &textMessage) const;

        //! Append if possible
        bool appendIfPossible(const CTextMessage &textMessage);

        //! Get SELCAL code (if applicable, e.g. ABCD), otherwise ""
        QString getSelcalCode() const;

        //! As icon, not implement by all classes
        CIcons::IconIndex toIcon() const;

        //! As pixmap, required for most GUI views
        QPixmap toPixmap() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CTextMessage &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! swift relay message marker
        static const QString &swiftRelayMessage();

    private:
        QString m_message;
        aviation::CCallsign m_senderCallsign;
        aviation::CCallsign m_recipientCallsign;
        physical_quantities::CFrequency m_frequency { 0, nullptr };
        bool m_wasSent = false; //!< transient
        bool m_relayedMessage = false; //!< transient

        SWIFT_METACLASS(
            CTextMessage,
            SWIFT_METAMEMBER(message),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch),
            SWIFT_METAMEMBER(senderCallsign),
            SWIFT_METAMEMBER(recipientCallsign),
            SWIFT_METAMEMBER(frequency));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CTextMessage)

#endif // guard
