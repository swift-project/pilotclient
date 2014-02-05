/* Copyright (C) 2013x VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTVOICE_INTERFACE_H
#define BLACKCORE_CONTEXTVOICE_INTERFACE_H

#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/vaudiodevicelist.h"
#include "blackmisc/vvoiceroomlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avcallsignlist.h"
#include "blackcore/voice_vatlib.h"
#include <QObject>
#include <QDBusAbstractInterface>

#define BLACKCORE_CONTEXTVOICE_INTERFACENAME "blackcore.contextvoice"
#define BLACKCORE_CONTEXTVOICE_SERVICEPATH "/voice"

// SERVICENAME must contain at least one ".", otherwise generation fails
// as this is interpreted in the way comain.somename

namespace BlackCore
{

    /*!
     * \brief The IContextVoice class
     */
    class IContextVoice : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTVOICE_INTERFACENAME)

    public:
        /*!
         * \brief Service name
         * \return
         */
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTVOICE_INTERFACENAME);
            return s;
        }

        /*!
         * \brief Service path
         * \return
         */
        static const QString &ServicePath()
        {
            static QString s(BLACKCORE_CONTEXTVOICE_SERVICEPATH);
            return s;
        }

        /*!
         * \brief DBus version constructor
         * \param serviceName
         * \param connection
         * \param parent
         */
        IContextVoice(const QString &serviceName, QDBusConnection &connection, QObject *parent = nullptr);

        /*!
         * Destructor
         */
        ~IContextVoice() {}

        /*!
         * \brief Using local objects?
         * \return
         */
        virtual bool usingLocalObjects() const { return false; }

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        /*!
         * Relay connection signals to local signals
         * No idea why this has to be wired and is not done automatically
         * \param connection
         */
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        /*!
         * \brief IContextVoice
         * \param parent
         */
        IContextVoice(QObject *parent = nullptr) : QObject(parent), m_dBusInterface(nullptr) {}

        /*!
         * \brief Helper for logging, likely to be removed / changed
         * \param method
         * \param m1
         * \param m2
         * \param m3
         * \param m4
         */
        void log(const QString &method, const QString &m1 = "", const QString &m2 = "", const QString &m3 = "", const QString &m4 = "") const;

    public slots:

        /*!
         * Get voice rooms for COM1, COM2:
         * From this connection audio status can be obtained
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const;

        /*!
         * Get voice rooms for COM1, COM2, but without latest audio status
         * \return all voice rooms
         */
        virtual BlackMisc::Voice::CVoiceRoomList getComVoiceRooms() const;

        /*!
         * \brief COM 1 voice room
         * \param withAudioStatus   update audio status
         * \return
         */
        virtual BlackMisc::Voice::CVoiceRoom getCom1VoiceRoom(bool withAudioStatus) const;

        /*!
         * \brief COM 2 voice room
         * \param withAudioStatus   update audio status
         * \return
         */
        virtual BlackMisc::Voice::CVoiceRoom getCom2VoiceRoom(bool withAudioStatus) const;

        /*!
         * \brief Set voice rooms
         */
        virtual void setComVoiceRooms(const BlackMisc::Voice::CVoiceRoom &voiceRoomCom1, const BlackMisc::Voice::CVoiceRoom &voiceRoomCom2);

        /*!
         * Leave all voice rooms
         */
        virtual void leaveAllVoiceRooms();

        /*!
         * \brief COM1 room users callsigns
         */
        virtual BlackMisc::Aviation::CCallsignList getCom1RoomCallsigns() const;

        /*!
         * \brief COM2 room users callsigns
         */
        virtual BlackMisc::Aviation::CCallsignList getCom2RoomCallsigns() const;

        /*!
         * \brief COM1 room users
         */
        virtual BlackMisc::Network::CUserList getCom1RoomUsers() const;

        /*!
         * \brief COM2 room users
         */
        virtual BlackMisc::Network::CUserList getCom2RoomUsers() const;

        /*!
         * \brief Audio devices
         */
        virtual BlackMisc::Voice::CAudioDeviceList getAudioDevices() const;

        /*!
         * \brief Get current audio device
         * \return input and output devices
         */
        virtual BlackMisc::Voice::CAudioDeviceList getCurrentAudioDevices() const;

        /*!
         * \brief Set current audio device
         * \param audioDevice can be input or audio device
         */
        virtual void setCurrentAudioDevice(const BlackMisc::Voice::CAudioDevice &audioDevice);

        /*!
         * \brief Set volumes via com units, also allows to mute
         * \see BlackMisc::Aviation::CComSystem::setVolumeInput()
         * \see BlackMisc::Aviation::CComSystem::setVolumeOutput()
         */
        virtual void setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2);

        /*!
         * \brief Is muted?
         */
        virtual bool isMuted() const;

        /*!
         * \brief Play SELCAL tone
         */
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const;
    };
}

#endif // guard
