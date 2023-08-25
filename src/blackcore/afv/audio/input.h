// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AFV_AUDIO_AUDIO_INPUT_H
#define BLACKCORE_AFV_AUDIO_AUDIO_INPUT_H

#include "blacksound/sampleprovider/bufferedwaveprovider.h"
#include "blacksound/codecs/opusencoder.h"
#include "blackmisc/audio/audiodeviceinfo.h"

#ifdef Q_OS_MAC
#    include "blackmisc/macos/microphoneaccess.h"
#endif

#include <QAudioInput>
#include <QString>
#include <QDateTime>
#include <QSharedPointer>

namespace BlackCore::Afv::Audio
{
    //! Input buffer
    class CAudioInputBuffer : public QIODevice
    {
        Q_OBJECT

    public:
        //! Inout buffer
        CAudioInputBuffer(QObject *parent);

        //! Start
        void start(const QAudioFormat &format);

        //! Stop
        void stop();

        //! \copydoc QIODevice::readData
        virtual qint64 readData(char *data, qint64 maxlen) override;

        //! \copydoc QIODevice::writeData
        virtual qint64 writeData(const char *data, qint64 len) override;

    signals:
        //! Frame is available
        void frameAvailable(const QByteArray &frame);

    private:
        static constexpr qint64 frameSize = 960;
        QByteArray m_buffer;
        QAudioFormat m_format;
    };

    //! Opus data arguments
    struct OpusDataAvailableArgs
    {
        uint sequenceCounter = 0; //!< sequence counter
        QByteArray audio; //!< audio data
    };

    //! Input volume stream arguments
    struct InputVolumeStreamArgs
    {
        //! Peak volume raw
        double PeakRaw = 0.0;

        //! Peak volume in dB
        double PeakDB = -1.0 * std::numeric_limits<double>::infinity();

        //! Peak volume in VU
        double PeakVU = 0.0;
    };

    //! Input
    class CInput : public QObject
    {
        Q_OBJECT

    public:
        //! Ctor
        CInput(int sampleRate, QObject *parent = nullptr);

        //! Dtor
        virtual ~CInput() override
        {
            this->stop();
        }

        //! @{
        //! Number of encoded bytes
        int opusBytesEncoded() const { return m_opusBytesEncoded; }
        void setOpusBytesEncoded(int opusBytesEncoded) { m_opusBytesEncoded = opusBytesEncoded; }
        //! @}

        //! @{
        //! Gain ratio, value a amplitude need to be multiplied with
        //! \see http://www.sengpielaudio.com/calculator-amplification.htm
        //! \remark gain ratio is voltage ratio/or amplitude ratio, something between 0.001-7.95 for -60dB to 80dB
        double getGainRatio() const { return m_gainRatio; }
        bool setGainRatio(double gainRatio);
        //! @}
        // those used to be the original function names
        // double volume() const { return m_volume; }
        // bool setVolume(double volume);

        /* disabled as not needed
        //! @{
        //! The device's volume 0..1
        double getDeviceInputVolume() const;
        bool setDeviceInputVolume(double volume);
        //! @}
        */

        //! Started?
        bool started() const { return m_started; }

        //! Start
        void start(const BlackMisc::Audio::CAudioDeviceInfo &inputDevice);

        //! Stop
        void stop();

        //! Corresponding device
        const BlackMisc::Audio::CAudioDeviceInfo &device() const { return m_device; }

    signals:
        //! Volume stream data
        void inputVolumeStream(const InputVolumeStreamArgs &args);

        //! OPUS data
        void opusDataAvailable(const OpusDataAvailableArgs &args);

    private:
        void audioInDataAvailable(const QByteArray &frame);

        static constexpr qint64 c_frameSize = 960;
        int m_sampleRate = 0;

        BlackSound::Codecs::COpusEncoder m_encoder;
        QScopedPointer<QAudioInput> m_audioInput;
        BlackMisc::Audio::CAudioDeviceInfo m_device;
        QAudioFormat m_inputFormat;

        bool m_started = false;
        int m_opusBytesEncoded = 0;
        int m_sampleCount = 0;
        double m_gainRatio = 1.0;
        qint16 m_maxSampleInput = 0.0;

        const int SampleCountPerEvent = 4800;
        const double maxDb = 0;
        const double minDb = -40;

        uint m_audioSequenceCounter = 0;
        CAudioInputBuffer *m_audioInputBuffer = nullptr;

#ifdef Q_OS_MAC
        BlackMisc::CMacOSMicrophoneAccess m_micAccess;
        void delayedInitMicrophone();
#endif
    };
} // ns

#endif // guard
