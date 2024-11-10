// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AFV_AUDIO_SOUNDCARDSAMPLEPROVIDER_H
#define BLACKCORE_AFV_AUDIO_SOUNDCARDSAMPLEPROVIDER_H

#include "sound/sampleprovider/sampleprovider.h"
#include "sound/sampleprovider/mixingsampleprovider.h"
#include "blackcore/afv/audio/receiversampleprovider.h"
#include "blackmisc/aviation/callsignset.h"

#include <QAudioFormat>
#include <QObject>

namespace BlackCore::Afv::Audio
{
    //! Soundcard sample
    class CSoundcardSampleProvider : public swift::sound::sample_provider::ISampleProvider
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

        //! \copydoc swift::sound::sample_provider::ISampleProvider::readSamples
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
        swift::sound::sample_provider::CMixingSampleProvider *m_mixer = nullptr;
        QVector<CReceiverSampleProvider *> m_receiverInputs;
        QVector<quint16> m_receiverIDs;
    };

} // ns

#endif // guard
