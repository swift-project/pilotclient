/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwtextmessagelist.h"
#include "predicates.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Empty constructor
         */
        CTextMessageList::CTextMessageList() { }

        /*
         * Single private message constructor
         */
        CTextMessageList::CTextMessageList(const QString &message, const CCallsign &recipientCallsign)
        {
            CTextMessage pm(message, recipientCallsign);
            this->push_back(pm);
        }

        /*
         * Single private message
         */
        CTextMessageList::CTextMessageList(const QString &message, const CCallsign &fromCallsign, const CCallsign &toCallsign)
        {
            CTextMessage pm(message, fromCallsign, toCallsign);
            this->push_back(pm);
        }

        /*
         * Single radio message
         */
        CTextMessageList::CTextMessageList(const QString &message, const CFrequency &frequency, const CCallsign &fromCallsign)
        {
            CTextMessage pm(message, frequency, fromCallsign);
            this->push_back(pm);
        }

        /*
         * Single text message
         */
        CTextMessageList::CTextMessageList(const CTextMessage &message)
        {
            this->push_back(message);
        }

        /*
         * Radio messages
         */
        CTextMessageList::CTextMessageList(const QString &message, const QList<CFrequency> &frequencies, const BlackMisc::Aviation::CCallsign &fromCallsign)
        {
            if (frequencies.isEmpty()) return;
            foreach(CFrequency frequency, frequencies)
            {
                CTextMessage pm(message, frequency, fromCallsign);
                this->push_back(pm);
            }
        }

        /*
         * Construct from base class object
         */
        CTextMessageList::CTextMessageList(const CSequence<CTextMessage> &other) :
            CSequence<CTextMessage>(other)
        { }

        /*
         * Private messages
         */
        CTextMessageList CTextMessageList::getPrivateMessages() const
        {
            return this->findBy(&CTextMessage::isPrivateMessage, true);
        }

        /*
         * Private messages?
         */
        bool CTextMessageList::containsPrivateMessages() const
        {
            return this->contains(&CTextMessage::isPrivateMessage, true);
        }

        /*
         * Radio messages
         */
        CTextMessageList CTextMessageList::getRadioMessages() const
        {
            return this->findBy(&CTextMessage::isRadioMessage, true);
        }

        /*
         * Radio messages?
         */
        bool CTextMessageList::containsRadioMessages() const
        {
            return this->contains(&CTextMessage::isRadioMessage, true);
        }

        /*
         * Register metadata
         */
        void CTextMessageList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CTextMessage>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CTextMessage>>();
            qRegisterMetaType<BlackMisc::CCollection<CTextMessage>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CTextMessage>>();
            qRegisterMetaType<CTextMessageList>();
            qDBusRegisterMetaType<CTextMessageList>();
        }

        /*
         * Find by frequency
         */
        CTextMessageList CTextMessageList::findByFrequency(const CFrequency &frequency) const
        {
            return this->findBy(&CTextMessage::getFrequency, frequency);
        }

        /*
         * Toggle all senders / receivers
         */
        void CTextMessageList::toggleSenderRecipients()
        {
            if (this->isEmpty()) return;
            std::for_each(this->begin(), this->end(), [](CTextMessage &tm) { tm.toggleSenderRecipient(); });
        }

    } // namespace
} // namespace
