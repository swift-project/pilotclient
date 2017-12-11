/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackcore/vatsim/audiodevicevatlib.h"
#include "blackmisc/logmessage.h"

#include <QString>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    namespace Vatsim
    {
        //! Cast void* to a pointer of CAudioInputDeviceVatlib
        CAudioInputDeviceVatlib *cbvar_cast_inputDevice(void *cbvar)
        {
            return static_cast<CAudioInputDeviceVatlib *>(cbvar);
        }

        //! Cast void* to a pointer of CAudioOutputDeviceVatlib
        CAudioOutputDeviceVatlib *cbvar_cast_outputDevice(void *cbvar)
        {
            return static_cast<CAudioOutputDeviceVatlib *>(cbvar);
        }

        CAudioInputDeviceVatlib::CAudioInputDeviceVatlib(VatAudioService *audioService, QObject *parent)
            : IAudioInputDevice(parent)
        {
            m_inputCodec.reset(Vat_CreateLocalInputCodec(audioService, vatCodecLegacy));
            Vat_GetInputDeviceInfo(m_inputCodec.data(), onInputHardwareDeviceReceived, this, nullptr);
            m_currentDevice = getDefaultInputDevice();
        }

        const BlackMisc::Audio::CAudioDeviceInfoList &CAudioInputDeviceVatlib::getInputDevices() const
        {
            return m_devices;
        }

        const BlackMisc::Audio::CAudioDeviceInfo &CAudioInputDeviceVatlib::getDefaultInputDevice() const
        {
            static BlackMisc::Audio::CAudioDeviceInfo info(BlackMisc::Audio::CAudioDeviceInfo::InputDevice, BlackMisc::Audio::CAudioDeviceInfo::defaultDeviceIndex(), "default");
            return info;
        }

        const BlackMisc::Audio::CAudioDeviceInfo &CAudioInputDeviceVatlib::getCurrentInputDevice() const
        {
            return m_currentDevice;
        }

        void CAudioInputDeviceVatlib::setInputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device)
        {
            Q_ASSERT_X(m_inputCodec, "CAudioInputDeviceVatlib", "VatLocalCodec is invalid!");
            if (!device.isValid())
            {
                CLogMessage(this).warning("Cannot set invalid input device!");
                return;
            }

            if (!Vat_SetAudioInputDevice(m_inputCodec.data(), device.getIndex()))
            {
                CLogMessage(this).warning("Setting input device failed");
            }
            m_currentDevice = device;
        }

        void CAudioInputDeviceVatlib::onInputHardwareDeviceReceived(int deviceIndex, const char *hardwareName, void *cbVar)
        {
            BlackMisc::Audio::CAudioDeviceInfo inputDevice(BlackMisc::Audio::CAudioDeviceInfo::InputDevice, deviceIndex, QString(hardwareName));
            cbvar_cast_inputDevice(cbVar)->m_devices.push_back(inputDevice);
        }

        CAudioOutputDeviceVatlib::CAudioOutputDeviceVatlib(VatAudioService *audioService, QObject *parent)
            : IAudioOutputDevice(parent)
        {
            m_outputCodec.reset(Vat_CreateLocalOutputCodec(audioService, vatCodecLegacy));
            Vat_GetOutputDeviceInfo(m_outputCodec.data(), onOutputHardwareDeviceReceived, this, nullptr);
            m_currentDevice = getDefaultOutputDevice();
        }

        const BlackMisc::Audio::CAudioDeviceInfoList &CAudioOutputDeviceVatlib::getOutputDevices() const
        {
            return m_devices;
        }

        const BlackMisc::Audio::CAudioDeviceInfo &CAudioOutputDeviceVatlib::getDefaultOutputDevice() const
        {
            static BlackMisc::Audio::CAudioDeviceInfo info(BlackMisc::Audio::CAudioDeviceInfo::OutputDevice, BlackMisc::Audio::CAudioDeviceInfo::defaultDeviceIndex(), "default");
            return info;
        }

        const BlackMisc::Audio::CAudioDeviceInfo &CAudioOutputDeviceVatlib::getCurrentOutputDevice() const
        {
            return m_currentDevice;
        }

        void CAudioOutputDeviceVatlib::setOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device)
        {
            Q_ASSERT_X(m_outputCodec, "CAudioOutputDeviceVatlib", "VatLocalCodec is invalid!");
            if (!device.isValid())
            {
                CLogMessage(this).warning("Cannot set invalid output device!");
                return;
            }

            Vat_SetAudioOutputDevice(m_outputCodec.data(), device.getIndex());
            this->m_currentDevice = device;
        }

        void CAudioOutputDeviceVatlib::setOutputVolume(int volume)
        {
            Q_ASSERT(m_outputCodec);
            Vat_SetOutputVolume(m_outputCodec.data(), volume);
        }

        int CAudioOutputDeviceVatlib::getOutputVolume() const
        {
            Q_ASSERT(m_outputCodec);
            return Vat_GetOutputVolume(m_outputCodec.data());
        }

        void CAudioOutputDeviceVatlib::onOutputHardwareDeviceReceived(int deviceIndex, const char *hardwareName, void *cbVar)
        {
            BlackMisc::Audio::CAudioDeviceInfo outputDevice(BlackMisc::Audio::CAudioDeviceInfo::OutputDevice, deviceIndex, QString(hardwareName));
            cbvar_cast_outputDevice(cbVar)->m_devices.push_back(outputDevice);
        }
    } // ns
} // ns
//! \endcond
