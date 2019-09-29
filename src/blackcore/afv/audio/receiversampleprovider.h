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

#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/audio/audiosettings.h"

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
                void setFrequency(const uint &frequencyHz);

                //! Number of active callsign
                int activeCallsigns() const;

                //! Volume
                double volume() const { return 1.0; }

                //! Mute @{
                bool getMute() const { return m_mute; }
                void setMute(bool value);
                //! @}

                //! \copydoc BlackSound::SampleProvider::ISampleProvider::readSamples
                virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

                void addOpusSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);
                void addSilentSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);

                quint16 getId() const { return m_id; }

                //! Receiving callsigns as string
                //! \remark those callsigns are transmitting and "I do receive them"
                const QString &getReceivingCallsignsString() const { return m_receivingCallsignsString; }

                //! Receiving callsigns
                //! \remark those callsigns are transmitting and "I do receive them"
                const BlackMisc::Aviation::CCallsignSet &getReceivingCallsigns() { return m_receivingCallsigns; }

            signals:
                //! Receving callsigns have changed
                void receivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args);

            private:
                uint m_frequencyHz = 122800000;
                bool m_mute = false;

                const double m_clickGain = 1.0;
                const double m_blockToneGain = 0.10;

                quint16 m_id;
                BlackMisc::CSettingReadOnly<BlackMisc::Audio::TSettings> m_audioSettings { this };

                // TODO VolumeSampleProvider volume;
                BlackSound::SampleProvider::CMixingSampleProvider *m_mixer = nullptr;
                // TODO SignalGenerator blockTone;
                QVector<CallsignSampleProvider *> m_voiceInputs;

                QString m_receivingCallsignsString;
                BlackMisc::Aviation::CCallsignSet m_receivingCallsigns;

                bool m_doClickWhenAppropriate = false;
                int m_lastNumberOfInUseInputs = 0;
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs)

#endif // guard
