/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_TEXTMESSAGELIST_H
#define BLACKMISC_TEXTMESSAGELIST_H

#include "nwtextmessage.h"
#include "collection.h"
#include "sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating a list of text messages
         */
        class CTextMessageList : public CSequence<CTextMessage>
        {
        public:
            /*!
             * \brief Empty constructor.
             */
            CTextMessageList();

            /*!
             * \brief Constructor, single private message
             * \param message
             * \param recipientCallsign
             */
            CTextMessageList(const QString &message, const BlackMisc::Aviation::CCallsign &recipientCallsign);

            /*!
             * \brief Constructor, single private message
             * \param message
             * \param senderCallsign
             * \param recipientCallsign
             */
            CTextMessageList(const QString &message, const BlackMisc::Aviation::CCallsign &senderCallsign, const BlackMisc::Aviation::CCallsign &recipientCallsign);

            /*!
             * \brief Constructor, single radio message
             * \param message
             * \param frequency
             * \param sender
             */
            CTextMessageList(const QString &message, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Aviation::CCallsign &senderCallsign = BlackMisc::Aviation::CCallsign());

            /*!
             * \brief Constructor, single message
             * \param message
             */
            CTextMessageList(const CTextMessage &message);

            /*!
             * \brief Constructor, multi-frequency radio messages
             * \param message
             * \param frequencies
             * \param sender
             */
            CTextMessageList(const QString &message, const QList<BlackMisc::PhysicalQuantities::CFrequency> &frequencies, const BlackMisc::Aviation::CCallsign &sender = BlackMisc::Aviation::CCallsign());

            /*!
             * \brief Construct from a base class object.
             * \param other
             */
            CTextMessageList(const CSequence<CTextMessage> &other);

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Private messages
             * \return
             */
            CTextMessageList getPrivateMessages() const;

            /*!
             * \brief Contains private messages?
             * \return
             */
            bool containsPrivateMessages() const;

            /*!
             * \brief Public messages
             * \return
             */
            CTextMessageList getRadioMessages() const;

            /*!
             * \brief Contains radio messages?
             * \return
             */
            bool containsRadioMessages() const;

            /*!
             * \brief Find by frequency
             * \param frequency
             * \return
             */
            CTextMessageList findByFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) const;

            /*!
             * \brief Toggle all sender receivers
             */
            void toggleSenderRecipients();

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CTextMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CTextMessage>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CTextMessage>)

#endif //guard
