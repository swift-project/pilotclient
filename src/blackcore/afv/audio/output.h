// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AFV_AUDIO_OUTPUT_H
#define BLACKCORE_AFV_AUDIO_OUTPUT_H

#include "sound/sampleprovider/sampleprovider.h"
#include "misc/audio/audiodeviceinfo.h"

#include <QObject>
#include <QAudioSink>

namespace BlackCore::Afv::Audio
{
    //! Stream args
    struct OutputVolumeStreamArgs
    {
        double PeakRaw = 0.0; //!< raw peak
        double PeakDb = -1 * std::numeric_limits<double>::infinity(); //!< dB peak
        double PeakVU = 0.0; //!< VU peak
    };

    //! Output buffer
    class CAudioOutputBuffer : public QIODevice
    {
        Q_OBJECT

    public:
        //! Ctor
        CAudioOutputBuffer(swift::sound::sample_provider::ISampleProvider *sampleProvider, QObject *parent);

        //! Set the format
        void setAudioFormat(const QAudioFormat &format) { m_outputFormat = format; }

    signals:
        //! Volume stream
        void outputVolumeStream(const OutputVolumeStreamArgs &args);

    protected:
#ifdef Q_OS_WIN
        //! \copydoc QIODevice::bytesAvailable
        qint64 bytesAvailable() const override;
#endif

        //! \copydoc QIODevice::readData
        virtual qint64 readData(char *data, qint64 maxlen) override;

        //! \copydoc QIODevice::writeData
        virtual qint64 writeData(const char *data, qint64 len) override;

    private:
        swift::sound::sample_provider::ISampleProvider *m_sampleProvider = nullptr; //!< related provider

        static constexpr int SampleCountPerEvent = 4800;
        QAudioFormat m_outputFormat;
        float m_maxSampleOutput = 0.0;
        int m_sampleCount = 0;
        const double m_maxDb = 0;
        const double m_minDb = -40;
    };

    //! Output
    class COutput : public QObject
    {
        Q_OBJECT

    public:
        //! Ctor
        COutput(QObject *parent = nullptr);

        //! Dtor
        virtual ~COutput() override
        {
            this->stop();
        }

        //! Start output
        void start(const swift::misc::audio::CAudioDeviceInfo &outputDevice, swift::sound::sample_provider::ISampleProvider *sampleProvider);

        //! Stop output
        void stop();

        //! Corresponding device
        const swift::misc::audio::CAudioDeviceInfo &device() const { return m_device; }

        /* disabled as not used
        //! @{
        //! The device's volume 0..1
        double getDeviceOutputVolume() const;
        bool setDeviceOutputVolume(double volume);
        //! @}
        */

    signals:
        //! Streaming data
        void outputVolumeStream(const OutputVolumeStreamArgs &args);

    private:
        bool m_started = false;
        swift::misc::audio::CAudioDeviceInfo m_device;
        QScopedPointer<QAudioSink> m_audioOutput;
        CAudioOutputBuffer *m_audioOutputBuffer = nullptr;
    };
} // ns

#endif // guard
