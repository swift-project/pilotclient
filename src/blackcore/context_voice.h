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
         * \brief Constructor
         */
        CContextVoice(CCoreRuntime *runtime);

        /*!
         * \brief Destructor
         */
        virtual ~CContextVoice();

        /*!
         * \brief Register myself in DBus
         * \param server DBus server
         */
        void registerWithDBus(CDBusServer *server)
        {
            server->addObject(IContextVoice::ServicePath(), this);
        }

        /*!
         * \brief Runtime
         */
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        /*!
         * \brief Using local objects?
         */
        virtual bool usingLocalObjects() const { return true; }

    public slots:
        /*!
         * \copydoc IContextVoice::setOwnAircraft
         */
        virtual void setOwnAircraft(const BlackMisc::Aviation::CAircraft &ownAircraft);

        /*!
         * \copydoc IContextVoice::getComVoiceRooms()
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRooms() const;

        /*!
         * \copydoc IContextVoice::getComVoiceRoomsWithAudioStatus()
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const;

        /*!
         * \copydoc IContextVoice::getCom1VoiceRoom
         */
        virtual BlackMisc::Voice::CVoiceRoom getCom1VoiceRoom(bool withAudioStatus) const;

        /*!
         * \copydoc IContextVoice::getCom2VoiceRoom
         */
        virtual BlackMisc::Voice::CVoiceRoom getCom2VoiceRoom(bool withAudioStatus) const;

        /*!
         * \copydoc IContextVoice::setComVoiceRooms()
         */
        virtual void setComVoiceRooms(const BlackMisc::Voice::CVoiceRoom &voiceRoomCom1, const BlackMisc::Voice::CVoiceRoom &voiceRoomCom2);

        /*!
         * \copydoc IContextVoice::getCom1RoomCallsigns()
         */
        virtual BlackMisc::Aviation::CCallsignList getCom1RoomCallsigns() const;

        /*!
         * \copydoc IContextVoice::getCom2RoomCallsigns()
         */
        virtual BlackMisc::Aviation::CCallsignList getCom2RoomCallsigns() const;

        /*!
         * \copydoc IContextVoice::getCom1RoomUsers()
         */
        virtual BlackMisc::Network::CUserList getCom1RoomUsers() const;

        /*!
         * \copydoc IContextVoice::getCom2RoomUsers()
         */
        virtual BlackMisc::Network::CUserList getCom2RoomUsers() const;

        /*!
         * \copydoc IContextVoice::leaveAllVoiceRooms
         */
        virtual void leaveAllVoiceRooms();

        /*!
         * \copydoc IContextVoice::getAudioDevices()
         */
        virtual BlackMisc::Voice::CAudioDeviceList getAudioDevices() const;

        /*!
         * \copydoc IContextVoice::getCurrentAudioDevices()
         */
        virtual BlackMisc::Voice::CAudioDeviceList getCurrentAudioDevices() const;

        /*!
         * \copydoc IContextVoice::setCurrentAudioDevice()
         */
        virtual void setCurrentAudioDevice(const BlackMisc::Voice::CAudioDevice &audioDevice);

        /*!
         * \copydoc IContextVoice::setVolumes()
         */
        virtual void setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

        /*!
         * \copydoc IContextVoice::isMuted()
         */
        virtual bool isMuted() const;

        /*!
         * \copydoc IContextVoice::playSelcalTone()
         */
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const;

        /*!
         * \copydoc IContextVoice::runMicrophoneTest()
         */
        virtual void runMicrophoneTest() const;

        /*!
         * \copydoc IContextVoice::runSquelchTest()
         */
        virtual void runSquelchTest() const;


    private:
        CVoiceVatlib *m_voice; //!< underlying voice lib
    };
}

#endif // guard
