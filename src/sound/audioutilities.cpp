// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "audioutilities.h"

#include <QAudioInput>
#include <QAudioSink>
#include <QAudioSource>
#include <QMediaDevices>
#include <QStringBuilder>

#include "config/buildconfig.h"
#include "misc/audio/audiodeviceinfolist.h"

using namespace swift::config;
using namespace swift::misc::audio;

namespace swift::sound
{
    QVector<float> convertBytesTo32BitFloatPCM(const QByteArray &input)
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

    QVector<qint16> convertBytesTo16BitPCM(const QByteArray &input)
    {
        const int inputSamples = input.size() / 2; // 16 bit input, so 2 bytes per sample
        QVector<qint16> output;
        output.fill(0, inputSamples);
        for (int n = 0; n < inputSamples; n++)
        {
            output[n] = *reinterpret_cast<const qint16 *>(input.data() + n * 2);
        }
        return output;
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

    QAudioDevice getLowestLatencyDevice(const CAudioDeviceInfo &device, QAudioFormat &format)
    {
        if (device.isDefault() || !device.isValid())
        {
            const QAudioDevice defDevice = device.isInputDevice() ? QMediaDevices::defaultAudioInput() : QMediaDevices::defaultAudioOutput();
            Q_ASSERT_X(defDevice.isFormatSupported(format), Q_FUNC_INFO, "Device does not support format");
            return defDevice;
        }

        const QList<QAudioDevice> allQtDevices =
            device.isInputDevice() ?
                CAudioDeviceInfoList::allQtInputDevices() :
                CAudioDeviceInfoList::allQtOutputDevices();

        // Find the one with lowest latency.
        QList<QAudioDevice> supportedDevices;
        for (const QAudioDevice &d : allQtDevices)
        {
            if (d.description() == device.getName() && d.isFormatSupported(format))
            {
                supportedDevices.push_back(d);
            }
        }

        if (supportedDevices.isEmpty()) { return {}; }

        QAudioDevice deviceWithLowestLatency = supportedDevices.at(0);
        deviceWithLowestLatency = device.isInputDevice() ? QMediaDevices::defaultAudioInput() : QMediaDevices::defaultAudioOutput();

        if (supportedDevices.size() > 1)
        {
            int lowestBufferSize = std::numeric_limits<int>::max();
            for (const QAudioDevice &d : supportedDevices)
            {
                if (!d.isFormatSupported(format)) continue;
                int bufferSize = 0;
                if (device.getType() == CAudioDeviceInfo::InputDevice)
                {
                    QAudioSource input(d, format);
                    input.start();
                    input.stop();
                    bufferSize = input.bufferSize();
                }
                else
                {
                    QAudioSink output(d, format);
                    output.start();
                    output.stop();
                    bufferSize = output.bufferSize();
                }

                if (bufferSize < lowestBufferSize)
                {
                    deviceWithLowestLatency = d;
                    lowestBufferSize = bufferSize;
                }
            }
        }
        return deviceWithLowestLatency;
    }

    QAudioDevice getHighestCompatibleOutputDevice(const CAudioDeviceInfo &device, QAudioFormat &format)
    {
        if (device.isDefault()) { return CAudioDeviceInfoList::defaultQtOutputDevice(); }
        const QList<QAudioDevice> allQtDevices = CAudioDeviceInfoList::allQtOutputDevices();

        QList<QAudioDevice> supportedDevices;
        for (const QAudioDevice &d : allQtDevices)
        {
            if (d.description() == device.getName())
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
            return {};
        }

        // here we could "search the best device", currently only first is taken
        QAudioDevice usedDevice = supportedDevices.front();
        Q_ASSERT_X(usedDevice.isFormatSupported(format), Q_FUNC_INFO, "Device does not support format");
        return usedDevice;
    }

    QString toQString(const QAudioFormat &format)
    {
        return QStringLiteral("Sample rate: %1 channels: %2 sample format: %3 bytes/frame: %4")
            .arg(format.sampleRate())
            .arg(format.channelCount())
            .arg(format.sampleFormat())
            .arg(format.bytesPerFrame());
    }

    const QString &toQString(QSysInfo::Endian s)
    {
        static const QString l("little");
        static const QString b("big");
        switch (s)
        {
        case QSysInfo::BigEndian: return b;
        case QSysInfo::LittleEndian: return l;
        default: break;
        }
        static const QString u("??");
        return u;
    }

    const QString &toQString(QAudioFormat::SampleFormat e)
    {
        static const QString s("signed int 16");
        static const QString u("unsigned int 8");
        static const QString f("float");
        switch (e)
        {
        case QAudioFormat::Int16: return s;
        case QAudioFormat::UInt8: return u;
        case QAudioFormat::Float: return f;
        case QAudioFormat::Unknown:
        default: break;
        }
        static const QString unknown("unknown");
        return unknown;
    }

    double normalize0to100(double in)
    {
        if (in < 0) { return 0; }
        return (in >= 1.0) ? 1.0 : in;
    }

    qreal normalize0to100qr(double in)
    {
        return static_cast<qreal>(normalize0to100(in));
    }

    void occupyAudioInputDevice()
    {
        static const QAudioInput input(QMediaDevices::defaultAudioInput());
    }

} // namespace swift::sound
