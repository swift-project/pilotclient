/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_AUDIO_SOUNDCARDSAMPLEPROVIDER_H
#define BLACKCORE_AFV_AUDIO_SOUNDCARDSAMPLEPROVIDER_H

#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/sampleprovider/mixingsampleprovider.h"
#include "receiversampleprovider.h"

#include <QAudioFormat>
#include <QObject>

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            //! Soundcard sample
            class SoundcardSampleProvider : public ISampleProvider
            {
                Q_OBJECT

            public:
                //! Ctor
                SoundcardSampleProvider(int sampleRate, const QVector<quint16> &transceiverIDs, QObject *parent = nullptr);

                QAudioFormat waveFormat() const;

                void setBypassEffects(bool value);
                void pttUpdate(bool active, const QVector<TxTransceiverDto> &txTransceivers);
                virtual int readSamples(QVector<qint16> &samples, qint64 count) override;
                void addOpusSamples(const IAudioDto &audioDto, const QVector<RxTransceiverDto> &rxTransceivers);
                void updateRadioTransceivers(const QVector<TransceiverDto> &radioTransceivers);
                QString getReceivingCallsigns(quint16 transceiverID);

            signals:
                void receivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args);

            private:
                QAudioFormat m_waveFormat;
                MixingSampleProvider *m_mixer;
                QVector<ReceiverSampleProvider *> m_receiverInputs;
                QVector<quint16> m_receiverIDs;
            };

        } // ns
    } // ns
} // ns

#endif // guard
