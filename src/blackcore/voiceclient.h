/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "../blackmisc/context.h"
#include "../blackmisc/avcallsign.h"
#include "../blackmisc/vvoiceroom.h"

#include <vatlib/vatlib.h>

#include <QObject>

namespace BlackCore
{
    class IVoiceClient : public QObject
    {

        /* TODOS:
         * - Find a replacement for comUnit. Maybe map it to the ComUnit in the aircraft as a class
         * - Object: class encapsulating a hardware device (vatlib uses integer index, which is necessarily persistent)
         * - Settings: Settings classes to store hardware settings (squelch, background noise, hardware device)
         */


        Q_OBJECT

    protected:

        /*!
         * \brief Default constructor with parent
         * \param parent
         */
        IVoiceClient(QObject *parent = 0) : QObject(parent) {}

    public:
        BLACK_INTERFACE(BlackCore::IVoice)

        virtual ~IVoiceClient() {}

        virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void joinVoiceRoom(const uint32_t comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom) = 0;
        virtual void leaveVoiceRoom(const uint32_t comUnit) = 0;
        virtual void setVolume(const uint32_t comUnit, const uint32_t volumne) = 0;
        virtual void startTransmitting(const uint32_t comUnit) = 0;
        virtual void stopTransmitting(const uint32_t comUnit) = 0;
        virtual bool isReceiving(const uint32_t comUnit) = 0;
        virtual bool isConnected(const uint32_t comUnit) = 0;

        virtual void getUserList(const uint32_t comUnit) = 0;
        virtual void getInputDevices(const uint32_t comUnit) = 0;
        virtual void getOutputDevices(const uint32_t comUnit) = 0;

        virtual const BlackMisc::Voice::CVoiceRoom &voiceRoom (const uint32_t comUnit) = 0;

    signals:
        void notConnected(const uint32_t comUnit);
        void connecting(const uint32_t comUnit);
        void connected(const uint32_t comUnit);
        void connectionFailed(const uint32_t comUnit);
        void kicked(const uint32_t comUnit);
        void disconnecting(const uint32_t comUnit);
        void disconnected(const uint32_t comUnit);
        void userJoined(const uint32_t comUnit, const BlackMisc::Aviation::CCallsign &callsign);
        void userLeft(const uint32_t comUnit, const BlackMisc::Aviation::CCallsign &callsign);
        void audioStarted(const uint32_t comUnit);
        void audioStopped(const uint32_t comUnit);

        void userList(const uint32_t comIndex);

        void outputDeviceList();
        void inputDeviceList();

    public slots:

    };

} // namespace BlackCore

#endif // BLACKCORE_VOICE_H
