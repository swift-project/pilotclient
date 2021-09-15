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
#include "blackcore/afv/audio/receiversampleprovider.h"
#include "blackmisc/aviation/callsignset.h"

#include <QAudioFormat>
#include <QObject>

namespace BlackCore::Afv::Audio
{
    //! Soundcard sample
    class CSoundcardSampleProvider : public BlackSound::SampleProvider::ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CSoundcardSampleProvider(int sampleRate, const QVector<quint16> &transceiverIDs, QObject *parent = nullptr);

        //! Wave format
        const QAudioFormat &waveFormat() const { return m_waveFormat; }

        //! Bypass effects
        void setBypassEffects(bool value);

        //! Update PTT
        void pttUpdate(bool active, const QVector<TxTransceiverDto> &txTransceivers);

        //! \copydoc BlackSound::SampleProvider::ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! Add OPUS samples
        void addOpusSamples(const IAudioDto &audioDto, const QVector<RxTransceiverDto> &rxTransceivers);

        //! Update all tranceivers
        void updateRadioTransceivers(const QVector<TransceiverDto> &radioTransceivers);

        //! Receiving callsign as single string
        QString getReceivingCallsignsString(quint16 transceiverID) const;

        //! Receiving callsign as single string
        BlackMisc::Aviation::CCallsignSet getReceivingCallsigns(quint16 transceiverID) const;

        //! Setting gain for specified receiver
        bool setGainRatioForTransceiver(quint16 transceiverID, double gainRatio);

    signals:
        //! Changed callsigns
        void receivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args);

    private:
        QAudioFormat m_waveFormat;
        BlackSound::SampleProvider::CMixingSampleProvider *m_mixer = nullptr;
        QVector<CReceiverSampleProvider *> m_receiverInputs;
        QVector<quint16> m_receiverIDs;
    };

} // ns

#endif // guard
