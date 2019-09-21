/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_AUDIO_CALLSIGNSAMPLEPROVIDER_H
#define BLACKCORE_AFV_AUDIO_CALLSIGNSAMPLEPROVIDER_H

#include "blackcore/afv/dto.h"
#include "blacksound/sampleprovider/pinknoisegenerator.h"
#include "blacksound/sampleprovider/bufferedwaveprovider.h"
#include "blacksound/sampleprovider/mixingsampleprovider.h"
#include "blacksound/sampleprovider/equalizersampleprovider.h"
#include "blacksound/sampleprovider/sawtoothgenerator.h"
#include "blacksound/sampleprovider/simplecompressoreffect.h"
#include "blacksound/sampleprovider/resourcesoundsampleprovider.h"
#include "blacksound/codecs/opusdecoder.h"

#include <QAudioFormat>
#include <QSoundEffect>
#include <QSharedPointer>
#include <QTimer>
#include <QDateTime>

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            //! Callsign provide
            class CallsignSampleProvider : public BlackSound::SampleProvider::ISampleProvider
            {
                Q_OBJECT

            public:
                //! Ctor
                CallsignSampleProvider(const QAudioFormat &audioFormat, QObject *parent = nullptr);

                int readSamples(QVector<qint16> &samples, qint64 count) override;

                //! The callsign
                const QString &callsign() const { return m_callsign; }

                const QString &type() const { return m_type; }

                void active(const QString &callsign, const QString &aircraftType);
                void activeSilent(const QString &callsign, const QString &aircraftType);

                void clear();

                void addOpusSamples(const IAudioDto &audioDto, float distanceRatio);
                void addSilentSamples(const IAudioDto &audioDto);

                bool inUse() const;

                void setBypassEffects(bool bypassEffects);

            private:
                void timerElapsed();
                void idle();
                QVector<qint16> decodeOpus(const QByteArray &opusData);
                void setEffects(bool noEffects = false);

                QAudioFormat m_audioFormat;

                const double whiteNoiseGainMin = 0.15;  //0.01;
                const double acBusGainMin      = 0.003; //0.002;
                const int frameCount    = 960;
                const int idleTimeoutMs = 500;

                QString m_callsign;
                QString m_type;
                bool m_inUse = false;

                bool m_bypassEffects = false;

                float m_distanceRatio = 1.0;

                BlackSound::SampleProvider::CMixingSampleProvider        *mixer = nullptr;
                BlackSound::SampleProvider::CResourceSoundSampleProvider *crackleSoundProvider = nullptr;
                BlackSound::SampleProvider::CResourceSoundSampleProvider *whiteNoise = nullptr;
                BlackSound::SampleProvider::CSawToothGenerator           *acBusNoise = nullptr;
                BlackSound::SampleProvider::CSimpleCompressorEffect      *simpleCompressorEffect = nullptr;
                BlackSound::SampleProvider::CEqualizerSampleProvider     *voiceEq  = nullptr;
                BlackSound::SampleProvider::CBufferedWaveProvider        *audioInput = nullptr;
                QTimer m_timer;

                BlackSound::Codecs::COpusDecoder m_decoder;
                bool m_lastPacketLatch = false;
                QDateTime m_lastSamplesAddedUtc;
                bool m_underflow = false;
            };
        } // ns
    } // ns
} // ns

#endif // guard
