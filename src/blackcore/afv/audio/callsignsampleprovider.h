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
            class CReceiverSampleProvider;

            //! Callsign provider
            class CCallsignSampleProvider : public BlackSound::SampleProvider::ISampleProvider
            {
                Q_OBJECT

            public:
                //! Ctor
                CCallsignSampleProvider(const QAudioFormat &audioFormat, const BlackCore::Afv::Audio::CReceiverSampleProvider *receiver, QObject *parent = nullptr);

                //! Read samples
                int readSamples(QVector<float> &samples, qint64 count) override;

                //! The callsign
                const QString &callsign() const { return m_callsign; }

                //! Type
                const QString &type() const { return m_type; }

                //! Is active?
                //! @{
                void active(const QString &callsign, const QString &aircraftType);
                void activeSilent(const QString &callsign, const QString &aircraftType);
                //! @}

                //! Clean
                void clear();

                //! Add samples @{
                void addOpusSamples(const IAudioDto &audioDto, float distanceRatio);
                void addSilentSamples(const IAudioDto &audioDto);
                //! @}

                //! Callsign in use
                bool inUse() const { return m_inUse; }

                //! Bypass effects
                void setBypassEffects(bool bypassEffects);

            private:
                void timerElapsed();
                void idle();
                QVector<qint16> decodeOpus(const QByteArray &opusData);
                void setEffects(bool noEffects = false);

                QAudioFormat m_audioFormat;

                const double m_whiteNoiseGainMin   = 0.17;   //0.01;
                const double m_hfWhiteNoiseGainMin = 0.6;    //0.01;
                const double m_acBusGainMin        = 0.0028; //0.002;
                const int m_frameCount    = 960;
                const int m_idleTimeoutMs = 500;

                QString m_callsign;
                QString m_type;
                bool m_inUse = false;

                bool m_bypassEffects  = false;
                float m_distanceRatio = 1.0;
                const CReceiverSampleProvider                            *m_receiver               = nullptr;
                BlackSound::SampleProvider::CMixingSampleProvider        *m_mixer                  = nullptr;
                BlackSound::SampleProvider::CResourceSoundSampleProvider *m_crackleSoundProvider   = nullptr;
                BlackSound::SampleProvider::CResourceSoundSampleProvider *m_whiteNoise             = nullptr;
                BlackSound::SampleProvider::CResourceSoundSampleProvider *m_hfWhiteNoise           = nullptr;
                BlackSound::SampleProvider::CSawToothGenerator           *m_acBusNoise             = nullptr;
                BlackSound::SampleProvider::CSimpleCompressorEffect      *m_simpleCompressorEffect = nullptr;
                BlackSound::SampleProvider::CEqualizerSampleProvider     *m_voiceEqualizer         = nullptr;
                BlackSound::SampleProvider::CBufferedWaveProvider        *m_audioInput             = nullptr;
                QTimer *m_timer = nullptr;

                BlackSound::Codecs::COpusDecoder m_decoder;
                bool m_lastPacketLatch = false;
                QDateTime m_lastSamplesAddedUtc;
                bool m_underflow = false;
            };
        } // ns
    } // ns
} // ns

#endif // guard
