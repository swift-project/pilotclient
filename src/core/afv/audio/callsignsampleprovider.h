// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AFV_AUDIO_CALLSIGNSAMPLEPROVIDER_H
#define SWIFT_CORE_AFV_AUDIO_CALLSIGNSAMPLEPROVIDER_H

#include <QAudioFormat>
#include <QDateTime>
#include <QSoundEffect>
#include <QTimer>

#include "core/afv/dto.h"
#include "sound/codecs/opusdecoder.h"
#include "sound/sampleprovider/bufferedwaveprovider.h"
#include "sound/sampleprovider/equalizersampleprovider.h"
#include "sound/sampleprovider/mixingsampleprovider.h"
#include "sound/sampleprovider/resourcesoundsampleprovider.h"
#include "sound/sampleprovider/sawtoothgenerator.h"
#include "sound/sampleprovider/simplecompressoreffect.h"

namespace swift::core::afv::audio
{
    class CReceiverSampleProvider;

    //! Callsign provider
    class CCallsignSampleProvider : public swift::sound::sample_provider::ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CCallsignSampleProvider(const QAudioFormat &audioFormat,
                                const swift::core::afv::audio::CReceiverSampleProvider *receiver,
                                QObject *parent = nullptr);

        //! Read samples
        int readSamples(QVector<float> &samples, qint64 count) override;

        //! The callsign
        const QString &callsign() const { return m_callsign; }

        //! Type
        const QString &type() const { return m_aircraftType; }

        //! @{
        //! Is active?
        void active(const QString &callsign, const QString &aircraftType);
        void activeSilent(const QString &callsign, const QString &aircraftType);
        //! @}

        //! Clean
        void clear();

        //! @{
        //! Add samples
        void addOpusSamples(const IAudioDto &audioDto, float distanceRatio);
        void addSilentSamples(const IAudioDto &audioDto);
        //! @}

        //! Callsign in use
        bool inUse() const { return m_inUse; }

        //! Bypass effects
        void setBypassEffects(bool bypassEffects);

        //! Info
        QString toQString() const;

    private:
        void timerElapsed();
        void idle();
        QVector<qint16> decodeOpus(const QByteArray &opusData);
        void setEffects(bool noEffects = false);

        QAudioFormat m_audioFormat;

        const double m_whiteNoiseGainMin = 0.17; // 0.01;
        const double m_hfWhiteNoiseGainMin = 0.6; // 0.01;
        const double m_acBusGainMin = 0.0028; // 0.002;
        const int m_frameCount = 960;
        const int m_idleTimeoutMs = 500;

        QString m_callsign;
        QString m_aircraftType;
        bool m_inUse = false;

        bool m_bypassEffects = false;
        float m_distanceRatio = 1.0;
        const CReceiverSampleProvider *m_receiver = nullptr;
        swift::sound::sample_provider::CMixingSampleProvider *m_mixer = nullptr;
        swift::sound::sample_provider::CResourceSoundSampleProvider *m_crackleSoundProvider = nullptr;
        swift::sound::sample_provider::CResourceSoundSampleProvider *m_whiteNoise = nullptr;
        swift::sound::sample_provider::CResourceSoundSampleProvider *m_hfWhiteNoise = nullptr;
        swift::sound::sample_provider::CSawToothGenerator *m_acBusNoise = nullptr;
        swift::sound::sample_provider::CSimpleCompressorEffect *m_simpleCompressorEffect = nullptr;
        swift::sound::sample_provider::CEqualizerSampleProvider *m_voiceEqualizer = nullptr;
        swift::sound::sample_provider::CBufferedWaveProvider *m_audioInput = nullptr;
        QTimer *m_timer = nullptr;

        swift::sound::codecs::COpusDecoder m_decoder;
        bool m_lastPacketLatch = false;
        QDateTime m_lastSamplesAddedUtc;
        bool m_underflow = false;
    };
} // namespace swift::core::afv::audio

#endif // SWIFT_CORE_AFV_AUDIO_CALLSIGNSAMPLEPROVIDER_H
