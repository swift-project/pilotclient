/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_H
#define BLACKCORE_VOICE_H

#include "../blackmisc/avcallsignlist.h"
#include "../blackmisc/avselcal.h"
#include "../blackmisc/nwuserlist.h"
#include "../blackmisc/voiceroomlist.h"
#include "../blackmisc/audiodeviceinfolist.h"
#include "../blackmisc/statusmessage.h"

#include <vatlib/vatlib.h>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QMetaType>

namespace BlackCore
{
    class IVoiceChannel;

    /*!
     * Interface to a connection to a ATC voice server for use in flight simulation.
     *
     * \warning If an INetwork signal is connected to a slot, and that slot emits a signal
     *          which is connected to an INetwork slot, then at least one of those connections
     *          must be a Qt::QueuedConnection.
     *          Reason: IVoiceClient implementations are not re-entrant.
     */
    class IVoice : public QObject
    {

        /* TODOS:
         * - Find a replacement for comUnit. Maybe map it to the ComUnit in the aircraft as a class
         * - Settings: Settings classes to store hardware settings (squelch, background noise, hardware device)
         */

        Q_OBJECT

    protected:

        /*!
         * \brief Default constructor with parent
         * \param parent
         */
        IVoice(QObject *parent = nullptr);

    public:
        //! Virtual destructor.
        virtual ~IVoice() {}

        /*!
         * \brief Audio devices
         * \return
         */
        virtual const BlackMisc::Audio::CAudioDeviceInfoList &audioDevices() const = 0;

        /*!
         * \brief Default input device
         * \return
         */
        virtual const BlackMisc::Audio::CAudioDeviceInfo defaultAudioInputDevice() const = 0;

        /*!
         * \brief Default output device
         * \return
         */
        virtual const BlackMisc::Audio::CAudioDeviceInfo defaultAudioOutputDevice() const = 0;

        //! Get voice channel object
        virtual IVoiceChannel *getVoiceChannel(qint32 channelIndex) const = 0;

        /*!
         * \brief Current input device
         */
        virtual BlackMisc::Audio::CAudioDeviceInfo getCurrentInputDevice() const = 0;

        /*!
         * \brief Current output device
         */
        virtual BlackMisc::Audio::CAudioDeviceInfo getCurrentOutputDevice() const = 0;

        /*!
         * \brief Output device to be used
         */
        virtual void setOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) = 0;

        /*!
         * \brief Input device to be used
         */
        virtual void setInputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) = 0;

        /*!
         * \brief Enable audio loopback to route recorded voice from microphone to speakers
         * \param enable (default true)
         */
        virtual void enableAudioLoopback(bool enable = true) = 0;
    };

} // namespace BlackCore

#endif // guard
