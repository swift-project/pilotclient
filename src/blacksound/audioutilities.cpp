/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "audioutilities.h"
#include <QAudioInput>
#include <QAudioOutput>

using namespace BlackMisc::Audio;

namespace BlackSound
{
    QVector<float> convertBytesTo32BitFloatPCM(const QByteArray input)
    {
        int inputSamples = input.size() / 2; // 16 bit input, so 2 bytes per sample
        QVector<float> output;
        output.fill(0, inputSamples);

        for (int n = 0; n < inputSamples; n++)
        {
            output[n] = *reinterpret_cast<const qint16 *>(input.data() + n * 2);
            output[n] /= 32767.0f;
        }
        return output;
    }

    QVector<qint16> convertBytesTo16BitPCM(const QByteArray input)
    {
        int inputSamples = input.size() / 2; // 16 bit input, so 2 bytes per sample
        QVector<qint16> output;
        output.fill(0, inputSamples);
        for (int n = 0; n < inputSamples; n++)
        {
            output[n] = *reinterpret_cast<const qint16 *>(input.data() + n * 2);
        }
        return output;
    }

    QVector<qint16> convertFloatBytesTo16BitPCM(const QByteArray input)
    {
        Q_UNUSED(input)
        // qFatal("Not implemented");
        return {};
    }

    QVector<float> convertFromMonoToStereo(const QVector<float> &mono)
    {
        QVector<float> stereo;
        stereo.reserve(mono.size() * 2);
        for (float sample : mono)
        {
            stereo << sample;
            stereo << sample;
        }
        return stereo;
    }

    QVector<qint16> convertFromStereoToMono(const QVector<qint16> &stereo)
    {
        QVector<qint16> mono;
        mono.reserve(stereo.size() / 2);
        for (int i = 0; i < stereo.size(); i = i + 2)
        {
            mono.append(stereo.at(i));
        }
        return mono;
    }

    QVector<float> convertFromShortToFloat(const QVector<qint16> &input)
    {
        QVector<float> output;
        for (auto sample : input)
        {
            output.push_back(sample / 32768.0f);
        }
        return output;
    }

    QAudioDeviceInfo getLowestLatencyDevice(const CAudioDeviceInfo &device, QAudioFormat &format)
    {
        if (device.isDefault())
        {
            if (device.getType() == CAudioDeviceInfo::InputDevice) { return QAudioDeviceInfo::defaultInputDevice(); }
            else { return QAudioDeviceInfo::defaultOutputDevice(); }
        }

        QAudio::Mode mode = (device.getType() == CAudioDeviceInfo::InputDevice) ? QAudio::AudioInput : QAudio::AudioOutput;
        const QList<QAudioDeviceInfo> allQtDevices = QAudioDeviceInfo::availableDevices(mode);

        // Find the one with lowest latency.
        QList<QAudioDeviceInfo> supportedDevices;
        for (const QAudioDeviceInfo &d : allQtDevices)
        {
            if (d.deviceName() == device.getName())
            {
                if (!d.isFormatSupported(format))
                {
                    // Check whether the nearest format is acceptable for our needs
                    const QAudioFormat nearestFormat = d.nearestFormat(format);
                    if (nearestFormat.sampleRate()     != format.sampleRate() ||
                            nearestFormat.sampleSize() != format.sampleSize() ||
                            nearestFormat.sampleType() != format.sampleType() ||
                            nearestFormat.byteOrder()  != format.byteOrder()  ||
                            nearestFormat.codec()      != format.codec())
                    {
                        continue;
                    }
                }
                supportedDevices.push_back(d);
            }
        }

        if (supportedDevices.empty()) { return {}; }

        QAudioDeviceInfo deviceWithLowestLatency = supportedDevices.at(0);

        if (supportedDevices.size() > 1)
        {
            QAudioFormat nearestFormat = format;
            int lowestBufferSize = std::numeric_limits<int>::max();
            for (const QAudioDeviceInfo &d : supportedDevices)
            {
                int bufferSize = 0;
                if (device.getType() == CAudioDeviceInfo::InputDevice)
                {
                    QAudioInput input(d, d.nearestFormat(format));
                    input.start();
                    input.stop();
                    bufferSize = input.bufferSize();
                }
                else
                {
                    QAudioOutput output(d, d.nearestFormat(format));
                    output.start();
                    output.stop();
                    bufferSize = output.bufferSize();
                }

                if (bufferSize < lowestBufferSize)
                {
                    deviceWithLowestLatency = d;
                    nearestFormat = d.nearestFormat(format);
                    lowestBufferSize = bufferSize;
                }
            }
            format = nearestFormat;
        }
        return deviceWithLowestLatency;
    }

    QAudioDeviceInfo getHighestCompatibleOutputDevice(const CAudioDeviceInfo &device, QAudioFormat &format)
    {
        if (device.isDefault()) { return QAudioDeviceInfo::defaultOutputDevice(); }
        const QList<QAudioDeviceInfo> allQtDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

        QList<QAudioDeviceInfo> supportedDevices;
        for (const QAudioDeviceInfo &d : allQtDevices)
        {
            if (d.deviceName() == device.getName())
            {
                // exact match, format supported
                if (d.isFormatSupported(format)) { return d; }
            }
            supportedDevices.push_back(d);
        }

        // no suitable device
        if (supportedDevices.isEmpty())
        {
            format = QAudioFormat();
            return QAudioDeviceInfo();
        }

        // here we could "search the best device", currently only first is taken
        QAudioDeviceInfo usedDevice = supportedDevices.front();
        if (!usedDevice.isFormatSupported(format))
        {
            format = usedDevice.nearestFormat(format);
        }
        return usedDevice;
    }

} // ns
