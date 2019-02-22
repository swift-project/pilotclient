/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/iterator.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/range.h"

#include <QtGlobal>
#include <algorithm>
#include <tuple>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {

        CTextMessageList::CTextMessageList() { }

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

        CTextMessageList::CTextMessageList(const QString &message, const QList<CFrequency> &frequencies, const BlackMisc::Aviation::CCallsign &fromCallsign)
        {
            if (frequencies.isEmpty()) return;
            for (const CFrequency &frequency : frequencies)
            {
                CTextMessage pm(message, frequency, fromCallsign);
                this->push_back(pm);
            }
        }

        CTextMessageList::CTextMessageList(const CSequence<CTextMessage> &other) :
            CSequence<CTextMessage>(other)
        { }

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

        void CTextMessageList::toggleSenderRecipients()
        {
            if (this->isEmpty()) { return; }
            std::for_each(this->begin(), this->end(), [](CTextMessage & tm) { tm.toggleSenderRecipient(); });
        }

        void CTextMessageList::markAsSent()
        {
            std::for_each(this->begin(), this->end(), [](CTextMessage & tm) { tm.markAsSent(); });
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
} // namespace
