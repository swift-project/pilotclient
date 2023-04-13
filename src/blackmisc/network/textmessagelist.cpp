/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/range.h"

#include <algorithm>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CTextMessage, CTextMessageList)

namespace BlackMisc::Network
{

    CTextMessageList::CTextMessageList() {}

    CTextMessageList::CTextMessageList(const QString &message, const CCallsign &recipientCallsign)
    {
        CTextMessage pm(message, recipientCallsign);
        this->push_back(pm);
    }

    CTextMessageList::CTextMessageList(const QString &message, const CCallsign &fromCallsign, const CCallsign &toCallsign)
    {
        CTextMessage pm(message, fromCallsign, toCallsign);
        this->push_back(pm);
    }

    CTextMessageList::CTextMessageList(const QString &message, const CFrequency &frequency, const CCallsign &fromCallsign)
    {
        CTextMessage pm(message, frequency, fromCallsign);
        this->push_back(pm);
    }

    CTextMessageList::CTextMessageList(const CTextMessage &message)
    {
        this->push_back(message);
    }

    CTextMessageList::CTextMessageList(const QString &message, const QList<CFrequency> &frequencies, const CCallsign &fromCallsign)
    {
        if (frequencies.isEmpty()) return;
        for (const CFrequency &frequency : frequencies)
        {
            CTextMessage pm(message, frequency, fromCallsign);
            this->push_back(pm);
        }
    }

    CTextMessageList::CTextMessageList(const CSequence<CTextMessage> &other) : CSequence<CTextMessage>(other)
    {}

    CTextMessageList CTextMessageList::getPrivateMessages() const
    {
        return this->findBy(&CTextMessage::isPrivateMessage, true);
    }

    bool CTextMessageList::containsPrivateMessages() const
    {
        return this->contains(&CTextMessage::isPrivateMessage, true);
    }

    CTextMessageList CTextMessageList::containsSupervisorMessages() const
    {
        return this->findBy(&CTextMessage::isSupervisorMessage, true);
    }

    CTextMessageList CTextMessageList::getRadioMessages() const
    {
        return this->findBy(&CTextMessage::isRadioMessage, true);
    }

    CTextMessageList CTextMessageList::getSupervisorMessages() const
    {
        return this->findBy(&CTextMessage::isSupervisorMessage, true);
    }

    bool CTextMessageList::containsRadioMessages() const
    {
        return this->contains(&CTextMessage::isRadioMessage, true);
    }

    CTextMessageList CTextMessageList::findByFrequency(const CFrequency &frequency) const
    {
        return this->findBy(&CTextMessage::getFrequency, frequency);
    }

    CTextMessageList CTextMessageList::findBySender(const CCallsign &sender) const
    {
        return this->findBy(&CTextMessage::getSenderCallsign, sender);
    }

    CTextMessageList CTextMessageList::findByRecipient(const CCallsign &recipient) const
    {
        return this->findBy(&CTextMessage::getRecipientCallsign, recipient);
    }

    CTextMessageList CTextMessageList::findByNotForRecipient(const CCallsign &recipient) const
    {
        CTextMessageList result;
        if (recipient.isEmpty()) { return result; }
        for (const CTextMessage &m : *this)
        {
            if (m.getRecipientCallsign().isEmpty()) { continue; }
            if (m.getRecipientCallsign() != recipient)
            {
                result.push_back(m);
            }
        }
        return result;
    }

    void CTextMessageList::toggleSenderRecipients()
    {
        if (this->isEmpty()) { return; }
        std::for_each(this->begin(), this->end(), [](CTextMessage &tm) { tm.toggleSenderRecipient(); });
    }

    int CTextMessageList::relayedToPrivateMessages()
    {
        if (this->isEmpty()) { return 0; }
        int c = 0;
        for (CTextMessage &m : *this)
        {
            // Turn "relay" message into private message
            if (m.relayedMessageToPrivateMessage()) { c++; }
        }
        return c;
    }

    int CTextMessageList::removePrivateMessagesFromCallsign(const CCallsign &callsign)
    {
        if (this->isEmpty()) { return 0; }
        CTextMessageList r = this->withRemovedPrivateMessagesFromCallsign(callsign);
        const int c = this->size() - r.size();
        if (c < 1) { return 0; }
        *this = r;
        return c;
    }

    CTextMessageList CTextMessageList::withRelayedToPrivateMessages() const
    {
        if (this->isEmpty()) { return {}; }
        CTextMessageList copy = *this;
        copy.relayedToPrivateMessages();
        return copy;
    }

    CTextMessageList CTextMessageList::withRemovedPrivateMessagesFromCallsign(const CCallsign &callsign) const
    {
        if (this->isEmpty()) { return {}; }
        if (callsign.isEmpty()) { return *this; }
        CTextMessageList r;
        for (const CTextMessage &m : *this)
        {
            if (m.isPrivateMessage() && m.getSenderCallsign() == callsign) { continue; }
            r.push_back(m);
        }
        return r;
    }

    void CTextMessageList::markAsSent()
    {
        std::for_each(this->begin(), this->end(), [](CTextMessage &tm) { tm.markAsSent(); });
    }

    CTextMessageList CTextMessageList::markedAsSent()
    {
        if (this->isEmpty()) { return {}; }
        CTextMessageList copy = *this;
        copy.markAsSent();
        return copy;
    }

    void CTextMessageList::addConsolidatedTextMessage(const CTextMessage &message)
    {
        if (message.isEmpty()) { return; }
        for (CTextMessage &tm : *this)
        {
            if (tm.appendIfPossible(message)) { return; }
        }
        this->push_back(message);
    }
} // namespace
