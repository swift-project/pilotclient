/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_AUDIODEVICEVATLIB_H
#define BLACKCORE_VATSIM_AUDIODEVICEVATLIB_H

#include "blackcore/audiodevice.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "vatlib/vatlib.h"

#include <QObject>
#include <QScopedPointer>

namespace BlackCore
{
    namespace Vatsim
    {
        //! Audio Input Device
        class BLACKCORE_EXPORT CAudioInputDeviceVatlib : public IAudioInputDevice
        {
            Q_OBJECT

        public:

            //! Constructor
            CAudioInputDeviceVatlib(VatAudioService *audioService, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAudioInputDeviceVatlib() = default;

            //! \copydoc IAudioInputDevice::getInputDevices
            virtual const BlackMisc::Audio::CAudioDeviceInfoList &getInputDevices() const override;

            //! \copydoc IAudioInputDevice::getCurrentInputDevice
            virtual const BlackMisc::Audio::CAudioDeviceInfo &getCurrentInputDevice() const override;

            //! \copydoc IAudioInputDevice::setInputDevice
            virtual void setInputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) override;

            //! Get vatlib input device pointer
            VatLocalInputCodec *getVatLocalInputCodec() { return m_inputCodec.data(); }

        private:

            struct VatLocalInputCodecDeleter
            {
                static inline void cleanup(VatLocalInputCodec *obj)
                {
                    if (obj) Vat_DestroyLocalInputCodec(obj);
                }
            };

            static void onInputHardwareDeviceReceived(int deviceIndex, const char *hardwareName, void *cbVar);

            BlackMisc::Audio::CAudioDeviceInfoList m_devices; /*!< in and output devices */
            BlackMisc::Audio::CAudioDeviceInfo m_currentDevice;

            QScopedPointer<VatLocalInputCodec, VatLocalInputCodecDeleter> m_inputCodec;
        };

        //! Audio Output Device
        class CAudioOutputDeviceVatlib : public IAudioOutputDevice
        {
            Q_OBJECT

        public:

            //! Constructor
            CAudioOutputDeviceVatlib(VatAudioService *audioService, QObject *parent = nullptr);

            //! Destructor
            virtual ~CAudioOutputDeviceVatlib() = default;

            //! \copydoc IAudioOutputDevice::getOutputDevices
            virtual const BlackMisc::Audio::CAudioDeviceInfoList &getOutputDevices() const override;

            //! \copydoc IAudioOutputDevice::getCurrentOutputDevice
            virtual const BlackMisc::Audio::CAudioDeviceInfo &getCurrentOutputDevice() const override;

            //! \copydoc IAudioOutputDevice::setOutputDevice
            virtual void setOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device) override;

            //! \copydoc IAudioOutputDevice::setOutputVolume
            virtual void setOutputVolume(int volume) override;

            //! \copydoc IAudioOutputDevice::getOutputVolume
            virtual int getOutputVolume() const override;

            //! Get vatlib output device pointer
            VatLocalOutputCodec *getVatLocalOutputCodec() { return m_outputCodec.data(); }

        private:

            struct VatLocalOutputCodecDeleter
            {
                static inline void cleanup(VatLocalOutputCodec *obj)
                {
                    if (obj) Vat_DestroyLocalOutputCodec(obj);
                }
            };

            static void onOutputHardwareDeviceReceived(int deviceIndex, const char *hardwareName, void *cbVar);


            BlackMisc::Audio::CAudioDeviceInfoList m_devices; /*!< in and output devices */
            BlackMisc::Audio::CAudioDeviceInfo m_currentDevice;

            QScopedPointer<VatLocalOutputCodec, VatLocalOutputCodecDeleter> m_outputCodec;
        };
    } // ns
} // ns

#endif // guard
