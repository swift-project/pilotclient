/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTVOICE_H
#define BLACKCORE_CONTEXTVOICE_H

#include "blackcore/dbus_server.h"
#include "blackcore/voice_vatlib.h"
#include "blackcore/context_voice_interface.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/nwuserlist.h"
#include "blackcore/coreruntime.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QMap>
#include <QSet>

#define BLACKCORE_CONTEXTVOICE_INTERFACENAME "blackcore.contextvoice"

namespace BlackCore
{
    /*!
     * \brief Network context
     */
    class CContextVoice : public IContextVoice
    {
        // Register by same name, make signals sender independent
        // http://dbus.freedesktop.org/doc/dbus-faq.html#idp48032144
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTVOICE_INTERFACENAME)
        Q_OBJECT

    public:

        /*!
         * \brief With link to server
         * \param server
         */
        CContextVoice(CCoreRuntime *parent);

        /*!
         * \brief Destructor
         */
        virtual ~CContextVoice();

        /*!
         * \brief Register myself in DBus
         * \param server
         */
        void registerWithDBus(CDBusServer *server)
        {
            server->addObject(IContextVoice::ServicePath(), this);
        }

        /*!
         * \brief Runtime
         * \return
         */
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        /*!
         * \brief Using local objects?
         * \return
         */
        virtual bool usingLocalObjects() const { return true; }

    public slots:
        /*!
         * Get voice rooms for COM1, COM2, but not with the latest status
         * \return
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRooms() const;

        /*!
         * Get voice rooms for COM1, COM2: From this connection status
         * etc. can be obtained
         * \return
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRoomsWithAudioStatus();

        /*!
         * \brief Set voice rooms
         * \param voiceRoomCom1
         * \param voiceRoomCom2
         */
        virtual void setComVoiceRooms(const BlackMisc::Voice::CVoiceRoom &voiceRoomCom1, const BlackMisc::Voice::CVoiceRoom &voiceRoomCom2);

        /*!
         * \brief COM1 room user's callsigns
         * \return
         */
        virtual QList<BlackMisc::Aviation::CCallsign> getCom1RoomCallsigns() const;

        /*!
         * \brief COM2 room user's callsigns
         * \return
         */
        virtual QList<BlackMisc::Aviation::CCallsign> getCom2RoomCallsigns() const;

        /*!
         * Leave all voice rooms
         */
        virtual void leaveAllVoiceRooms();

        /*!
         * \brief Audio devices
         * \return
         */
        virtual BlackMisc::Voice::CAudioDeviceList getAudioDevices() const;

        /*!
         * \brief Set current audio device
         * \param audioDevice
         */
        virtual BlackMisc::Voice::CAudioDeviceList getCurrentAudioDevices() const;

        /*!
         * \brief Set current audio device
         * \param audioDevice
         */
        virtual void setCurrentAudioDevice(const BlackMisc::Voice::CAudioDevice &audioDevice);

        /*!
         * \brief Set volumes
         * \param com1
         * \param com2
         */
        virtual void setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

    private:
        CVoiceVatlib *m_voice;
        BlackMisc::Voice::CAudioDevice m_currentInputDevice;
        BlackMisc::Voice::CAudioDevice m_currentOutputDevice;
    };
}

#endif // guard
