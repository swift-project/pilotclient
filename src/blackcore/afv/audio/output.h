/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_AUDIO_OUTPUT_H
#define BLACKCORE_AFV_AUDIO_OUTPUT_H

#include "blacksound/sampleprovider/sampleprovider.h"
#include "blackmisc/audio/audiodeviceinfo.h"

#include <QObject>
#include <QAudioOutput>

namespace BlackCore::Afv::Audio
{
    //! Stream args
    struct OutputVolumeStreamArgs
    {
        double PeakRaw = 0.0; //!< raw peak
        double PeakDb  = -1 * std::numeric_limits<double>::infinity(); //!< dB peak
        double PeakVU  = 0.0; //!< VU peak
    };

    //! Output buffer
    class CAudioOutputBuffer : public QIODevice
    {
        Q_OBJECT

    public:
        //! Ctor
        CAudioOutputBuffer(BlackSound::SampleProvider::ISampleProvider *sampleProvider, QObject *parent);

        //! Set the format
        void setAudioFormat(const QAudioFormat &format) { m_outputFormat = format; }

    signals:
        //! Volume stream
        void outputVolumeStream(const OutputVolumeStreamArgs &args);

    protected:
        //! \copydoc QIODevice::readData
        virtual qint64 readData(char *data, qint64 maxlen) override;

        //! \copydoc QIODevice::writeData
        virtual qint64 writeData(const char *data, qint64 len) override;

    private:
        BlackSound::SampleProvider::ISampleProvider *m_sampleProvider = nullptr; //!< related provider

        static constexpr int SampleCountPerEvent = 4800;
        QAudioFormat m_outputFormat;
        float m_maxSampleOutput = 0.0;
        int m_sampleCount       =   0;
        const double m_maxDb    =   0;
        const double m_minDb    = -40;
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
        void start(const BlackMisc::Audio::CAudioDeviceInfo &outputDevice, BlackSound::SampleProvider::ISampleProvider *sampleProvider);

        //! Stop output
        void stop();

        //! Corresponding device
        const BlackMisc::Audio::CAudioDeviceInfo &device() const { return m_device; }

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
        BlackMisc::Audio::CAudioDeviceInfo m_device;
        QScopedPointer<QAudioOutput>       m_audioOutput;
        CAudioOutputBuffer                *m_audioOutputBuffer = nullptr;
    };
} // ns

#endif // guard
