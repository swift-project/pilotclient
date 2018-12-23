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
            int currentDeviceIndex;
            Vat_GetInputDeviceInfo(m_inputCodec.data(), onInputHardwareDeviceReceived, this, &currentDeviceIndex);
            m_currentDevice = m_devices.findByDeviceIndex(currentDeviceIndex);
        }

        const CAudioDeviceInfoList &CAudioInputDeviceVatlib::getInputDevices() const
        {
            return m_devices;
        }

        const CAudioDeviceInfo &CAudioInputDeviceVatlib::getCurrentInputDevice() const
        {
            return m_currentDevice;
        }

        void CAudioInputDeviceVatlib::setInputDevice(const CAudioDeviceInfo &device)
        {
            Q_ASSERT_X(m_inputCodec, "CAudioInputDeviceVatlib", "VatLocalCodec is invalid!");
            if (!device.isValid())
            {
                CLogMessage(this).warning(u"Cannot set invalid input device!");
                return;
            }

            if (!Vat_SetAudioInputDevice(m_inputCodec.data(), device.getIndex()))
            {
                CLogMessage(this).warning(u"Setting input device failed");
            }
            m_currentDevice = device;
        }

        void CAudioInputDeviceVatlib::onInputHardwareDeviceReceived(int deviceIndex, const char *hardwareName, void *cbVar)
        {
            CAudioDeviceInfo inputDevice(CAudioDeviceInfo::InputDevice, deviceIndex, QString(hardwareName));
            cbvar_cast_inputDevice(cbVar)->m_devices.push_back(inputDevice);
        }

        CAudioOutputDeviceVatlib::CAudioOutputDeviceVatlib(VatAudioService *audioService, QObject *parent)
            : IAudioOutputDevice(parent)
        {
            m_outputCodec.reset(Vat_CreateLocalOutputCodec(audioService, vatCodecLegacy));
            int currentDeviceIndex;
            Vat_GetOutputDeviceInfo(m_outputCodec.data(), onOutputHardwareDeviceReceived, this, &currentDeviceIndex);
            m_currentDevice = m_devices.findByDeviceIndex(currentDeviceIndex);
        }

        const CAudioDeviceInfoList &CAudioOutputDeviceVatlib::getOutputDevices() const
        {
            return m_devices;
        }

        const CAudioDeviceInfo &CAudioOutputDeviceVatlib::getCurrentOutputDevice() const
        {
            return m_currentDevice;
        }

        void CAudioOutputDeviceVatlib::setOutputDevice(const CAudioDeviceInfo &device)
        {
            Q_ASSERT_X(m_outputCodec, "CAudioOutputDeviceVatlib", "VatLocalCodec is invalid!");
            if (!device.isValid())
            {
                CLogMessage(this).warning(u"Cannot set invalid output device!");
                return;
            }

            Vat_SetAudioOutputDevice(m_outputCodec.data(), device.getIndex());
            m_currentDevice = device;
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
            CAudioDeviceInfo outputDevice(CAudioDeviceInfo::OutputDevice, deviceIndex, QString(hardwareName));
            cbvar_cast_outputDevice(cbVar)->m_devices.push_back(outputDevice);
        }
    } // ns
} // ns
//! \endcond
