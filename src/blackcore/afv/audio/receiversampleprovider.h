/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_AUDIO_RECEIVERSAMPLEPROVIDER_H
#define BLACKCORE_AFV_AUDIO_RECEIVERSAMPLEPROVIDER_H

#include "callsignsampleprovider.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/sampleprovider/mixingsampleprovider.h"

#include <QtGlobal>

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            //! Arguments
            struct TransceiverReceivingCallsignsChangedArgs
            {
                quint16 transceiverID;
                QStringList receivingCallsigns;
            };

            //! A sample provider
            class CReceiverSampleProvider : public BlackSound::SampleProvider::ISampleProvider
            {
                Q_OBJECT

            public:
                //! Ctor
                CReceiverSampleProvider(const QAudioFormat &audioFormat, quint16 id, int voiceInputNumber, QObject *parent = nullptr);

                void setBypassEffects(bool value);
                void setFrequency(const uint &frequency);
                int activeCallsigns() const;
                float volume() const;

                bool getMute() const;
                void setMute(bool value);

                virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

                void addOpusSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);
                void addSilentSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);

                quint16 getId() const { return m_id; }
                QString getReceivingCallsigns() const;

            signals:
                void receivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args);

            private:
                uint m_frequency = 122800;
                bool m_mute = false;

                const float m_clickGain = 1.0f;
                const double m_blockToneGain = 0.10f;

                quint16 m_id;

                // TODO VolumeSampleProvider volume;
                BlackSound::SampleProvider::CMixingSampleProvider *m_mixer = nullptr;
                // TODO SignalGenerator blockTone;
                QVector<CallsignSampleProvider *> m_voiceInputs;
                QString m_receivingCallsigns;

                bool m_doClickWhenAppropriate = false;
                int lastNumberOfInUseInputs = 0;
            };
        } // ns
    } // ns
} // ns

#endif // guard
