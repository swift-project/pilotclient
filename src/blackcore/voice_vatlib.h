/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VOICE_VATLIB_H
#define BLACKCORE_VOICE_VATLIB_H

#include "voice.h"
#include <vatlib/vatlib2.h>

#include <QString>
#include <QScopedPointer>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

//! \file

namespace BlackCore
{
    /*!
     * Vatlib implementation of the IVoice interface.
     */
    class CVoiceVatlib : public IVoice
    {
        Q_OBJECT

    public:

        /*!
         * \brief Constructor
         * \param parent
         */
        CVoiceVatlib(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CVoiceVatlib();

        // Hardware devices
        // TODO: Vatlib supports multiple output devices. That basically means, you could connect
        // to different voice rooms and send their audio to different devices, e.g. ATIS to loudspeakers
        // and ATC to headspeakers. Is not important to implement that now, if ever.

        //! \copydoc IVoice::audioDevices()
        virtual const BlackMisc::Audio::CAudioDeviceInfoList &audioDevices() const override;

        //! \copydoc IVoice::defaultAudioInputDevice()
        virtual const BlackMisc::Audio::CAudioDeviceInfo defaultAudioInputDevice() const override;

        //! \copydoc IVoice::defaultAudioOutputDevice()
        virtual const BlackMisc::Audio::CAudioDeviceInfo defaultAudioOutputDevice() const override;

        //! \copydoc IVoice::getVoiceChannel
        virtual IVoiceChannel *getVoiceChannel(qint32 channelIndex) const override;

        //! \copydoc IVoice::setInputDevice
        virtual void setInputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) override;

        //! \copydoc IVoice::setOutputDevice
        virtual void setOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) override;

        //! \copydoc IVoice::getCurrentInputDevice()
        virtual BlackMisc::Audio::CAudioDeviceInfo getCurrentInputDevice() const override;

        //! \copydoc IVoice::getCurrentOutputDevice()
        virtual BlackMisc::Audio::CAudioDeviceInfo getCurrentOutputDevice() const override;

        //! \copydoc IVoice::enableAudioLoopback
        virtual void enableAudioLoopback(bool enable = true) override;

        /*!
         * \brief Starts or stops voice transmission
         * \param value
         */
        void handlePushToTalk(bool value = false);

    protected: // QObject overrides

        /*!
         * \brief Process voice lib
         */
        virtual void timerEvent(QTimerEvent *);

    private:

        // this struct calls "myCustomDeallocator" to delete the pointer
        struct VatAudioServiceDeleter
        {
            static inline void cleanup(VatAudioService_tag *obj)
            {
                Vat_DestroyAudioService(obj);
            }
        };

        struct VatUDPAudioPortDeleter
        {
            static inline void cleanup(VatUDPAudioPort_tag *obj)
            {
                Vat_DestroyUDPAudioPort(obj);
            }
        };

        static void voiceErrorHandler(const char *message);

        // shimlib callbacks
        static void onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, qint32 roomIndex, void *cbVar);

        void onRoomStatusUpdate(qint32 roomIndex, Cvatlib_Voice_Simple::roomStatusUpdate roomStatus);

        QScopedPointer<VatAudioService_tag, VatAudioServiceDeleter> m_audioService;
        QScopedPointer<VatUDPAudioPort_tag, VatUDPAudioPortDeleter> m_udpPort;
        BlackMisc::Audio::CAudioDeviceInfoList m_devices; /*!< in and output devices */
        BlackMisc::Audio::CAudioDeviceInfo m_currentOutputDevice;
        BlackMisc::Audio::CAudioDeviceInfo m_currentInputDevice;
        QHash<qint32, IVoiceChannel *> m_hashChannelIndex;
        bool m_isAudioLoopbackEnabled; /*!< A flag whether audio loopback is enabled or not */

    };

} // namespace

#endif // guard
