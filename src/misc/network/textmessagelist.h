// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_TEXTMESSAGELIST_H
#define SWIFT_MISC_NETWORK_TEXTMESSAGELIST_H

#include <QList>
#include <QMetaType>
#include <QString>

#include "misc/aviation/callsign.h"
#include "misc/collection.h"
#include "misc/network/textmessage.h"
#include "misc/pq/frequency.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampobjectlist.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CTextMessage, CTextMessageList)

namespace swift::misc::network
{
    class CTextMessage;

    //! Value object encapsulating a list of text messages
    class SWIFT_MISC_EXPORT CTextMessageList :
        public CSequence<CTextMessage>,
        public ITimestampObjectList<CTextMessage, CTextMessageList>,
        public mixin::MetaType<CTextMessageList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CTextMessageList)
        using CSequence::CSequence;

        //! Default constructor.
        CTextMessageList() = default;

        //! Constructor, single private message
        CTextMessageList(const QString &message, const aviation::CCallsign &recipientCallsign);

        //! Constructor, single private message
        CTextMessageList(const QString &message, const aviation::CCallsign &senderCallsign,
                         const aviation::CCallsign &recipientCallsign);

        //! Constructor, single radio message
        CTextMessageList(const QString &message, const physical_quantities::CFrequency &frequency,
                         const aviation::CCallsign &senderCallsign = {});

        //! Constructor, single message
        CTextMessageList(const CTextMessage &message);

        //! Constructor, multi-frequency radio messages
        CTextMessageList(const QString &message, const QList<physical_quantities::CFrequency> &frequencies,
                         const aviation::CCallsign &fromCallsign = {});

        //! Construct from a base class object.
        CTextMessageList(const CSequence<CTextMessage> &other);

        //! Private messages
        CTextMessageList getPrivateMessages() const;

        //! Contains private messages?
        bool containsPrivateMessages() const;

        //! Public messages
        CTextMessageList getRadioMessages() const;

        //! Supervisor messages
        CTextMessageList getSupervisorMessages() const;

        //! Contains radio messages?
        bool containsRadioMessages() const;

        //! Contains supervisor message
        CTextMessageList containsSupervisorMessages() const;

        //! Find by frequency
        CTextMessageList findByFrequency(const physical_quantities::CFrequency &frequency) const;

        //! Find by sender
        CTextMessageList findBySender(const swift::misc::aviation::CCallsign &sender) const;

        //! Find by recipient
        CTextMessageList findByRecipient(const swift::misc::aviation::CCallsign &recipient) const;

        //! Find by recipient is NOT addressed
        CTextMessageList findByNotForRecipient(const swift::misc::aviation::CCallsign &recipient) const;

        //! Toggle all sender <-> recipients
        void toggleSenderRecipients();

        //! Turn relayed into normal private messages
        int relayedToPrivateMessages();

        //! Remove private messages from callsign
        int removePrivateMessagesFromCallsign(const aviation::CCallsign &callsign);

        //! List with relayed messages (if any) as private messages
        CTextMessageList withRelayedToPrivateMessages() const;

        //! With removed private messages from callsign
        CTextMessageList withRemovedPrivateMessagesFromCallsign(const aviation::CCallsign &callsign) const;

        //! Mark all messages as sent
        void markAsSent();

        //! Marked as sent
        CTextMessageList markedAsSent();

        //! Add a text message, but append it to an existing message if possible
        void addConsolidatedTextMessage(const CTextMessage &message);
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CTextMessageList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CTextMessage>)

#endif // SWIFT_MISC_NETWORK_TEXTMESSAGELIST_H
