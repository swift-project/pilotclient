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

        //! \brief Constructor
        CContextVoice(CCoreRuntime *runtime);

        //! \brief Destructor
        virtual ~CContextVoice();

        /*!
         * \brief Register myself in DBus
         * \param server DBus server
         */
        void registerWithDBus(CDBusServer *server)
        {
            server->addObject(IContextVoice::ServicePath(), this);
        }

        //! \brief Runtime
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

        //! \brief Using local objects?
        virtual bool usingLocalObjects() const { return true; }

    public slots:
        //! \copydoc IContextVoice::setOwnAircraft
        virtual void setOwnAircraft(const BlackMisc::Aviation::CAircraft &ownAircraft) override;

        //! \copydoc IContextVoice::getComVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const override;

        //! \copydoc IContextVoice::getComVoiceRoomsWithAudioStatus()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const override;

        //! \copydoc IContextVoice::getCom1VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom1VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextVoice::getCom2VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom2VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextVoice::setComVoiceRooms()
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoom &voiceRoomCom1, const BlackMisc::Audio::CVoiceRoom &voiceRoomCom2) override;

        //! \copydoc IContextVoice::getCom1RoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getCom1RoomCallsigns() const override;

        //! \copydoc IContextVoice::getCom2RoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getCom2RoomCallsigns() const override;

        //! \copydoc IContextVoice::getCom1RoomUsers()
        virtual BlackMisc::Network::CUserList getCom1RoomUsers() const override;

        //! \copydoc IContextVoice::getCom2RoomUsers()
        virtual BlackMisc::Network::CUserList getCom2RoomUsers() const override;

        //! \copydoc IContextVoice::leaveAllVoiceRooms
        virtual void leaveAllVoiceRooms() override;

        //! \copydoc IContextVoice::getAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceList getAudioDevices() const override;

        //! \copydoc IContextVoice::getCurrentAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceList getCurrentAudioDevices() const override;

        //! \copydoc IContextVoice::setCurrentAudioDevice()
        virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDevice &audioDevice) override;

        //! \copydoc IContextVoice::setVolumes()
        virtual void setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2) override;

        //! \copydoc IContextVoice::isMuted()
        virtual bool isMuted() const override;

        //! \copydoc IContextVoice::playSelcalTone()
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const override;

        //! \copydoc IContextVoice::playNotification()
        virtual void playNotification(uint notification) const override;

        //! \copydoc IContextVoice::runMicrophoneTest()
        virtual void runMicrophoneTest() override;

        //! \copydoc IContextVoice::runSquelchTest()
        virtual void runSquelchTest() override;

        //! \copydoc IContextVoice::getMicrophoneTestResult()
        virtual QString getMicrophoneTestResult() const override;

        //! \copydoc IContextVoice::getSquelchValue()
        virtual double getSquelchValue() const override;

    private:
        CVoiceVatlib *m_voice; //!< underlying voice lib
    };
}

#endif // guard
