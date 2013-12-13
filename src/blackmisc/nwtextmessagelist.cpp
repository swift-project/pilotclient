/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
        CTextMessageList::CTextMessageList(const QString &message, const CCallsign &toCallsign)
        {
            CTextMessage pm(message, toCallsign);
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
         * Find by callsign
         */
        CTextMessageList CTextMessageList::findByToCallsign(const CCallsign &callsign) const
        {
            return CTextMessageList(this->findBy(&CTextMessage::getToCallsign, callsign));
        }

        /*
         * Find by frequency
         */
        CTextMessageList CTextMessageList::findByFrequency(const CFrequency &frequency) const
        {
            return CTextMessageList(this->findBy(&CTextMessage::getFrequency, frequency));
        }

        /*
         * Toggle all senders / receivers
         */
        void CTextMessageList::toggleSenderReceivers()
        {
            if (this->isEmpty()) return;
            for (int i = 0; i < this->size(); i++)
            {
                CTextMessage tm = (*this)[i];
                tm.toggleSenderReceiver();
                (*this)[i] = tm;
            }
        }

    } // namespace
} // namespace
