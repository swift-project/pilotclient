// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AFV_AUDIO_RECEIVERSAMPLEPROVIDER_H
#define BLACKCORE_AFV_AUDIO_RECEIVERSAMPLEPROVIDER_H

#include "blackcore/afv/audio/callsignsampleprovider.h"
#include "sound/sampleprovider/sampleprovider.h"
#include "sound/sampleprovider/mixingsampleprovider.h"
#include "sound/sampleprovider/sinusgenerator.h"
#include "sound/sampleprovider/volumesampleprovider.h"

#include "misc/logcategories.h"
#include "misc/aviation/callsignset.h"
#include "misc/audio/audiosettings.h"

#include <QtGlobal>

namespace BlackCore::Afv::Audio
{
    //! Arguments
    struct TransceiverReceivingCallsignsChangedArgs
    {
        quint16 transceiverID; //!< transceiver id
        QStringList receivingCallsigns; //!< callsigns
    };

    //! A sample provider
    class CReceiverSampleProvider : public swift::sound::sample_provider::ISampleProvider
    {
        Q_OBJECT

    public:
        //! Log.categories
        static const QStringList &getLogCategories();

        //! Ctor
        CReceiverSampleProvider(const QAudioFormat &audioFormat, quint16 id, int voiceInputNumber, QObject *parent = nullptr);

        //! Bypass effects
        void setBypassEffects(bool value);

        //! Frequency
        void setFrequency(const uint &frequencyHz);

        //! Number of active callsign
        int activeCallsigns() const;

        //! Volume
        double volume() const { return 1.0; }

        //! @{
        //! Mute
        bool getMute() const { return m_mute; }
        void setMute(bool value);
        //! @}

        //! \copydoc swift::sound::sample_provider::ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! @{
        //! Add samples
        void addOpusSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);
        void addSilentSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio);
        //! @}

        //! ID
        quint16 getId() const { return m_id; }

        //! Receiving callsigns as string
        //! \remark those callsigns are transmitting and "I do receive them"
        const QString &getReceivingCallsignsString() const { return m_receivingCallsignsString; }

        //! Receiving callsigns
        //! \remark those callsigns are transmitting and "I do receive them"
        const swift::misc::aviation::CCallsignSet &getReceivingCallsigns() { return m_receivingCallsigns; }

        //! Get frequency in Hz
        uint getFrequencyHz() const;

        //! Set gain ratio
        bool setGainRatio(double gainRatio) { return m_volume->setGainRatio(gainRatio); }

        //! Log all inputs
        //! \private DEBUG only
        void logVoiceInputs(const QString &prefix = {}, qint64 timeCheckOffsetMs = -1);

    signals:
        //! Receving callsigns have changed
        void receivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args);

    private:
        uint m_frequencyHz = 122800000;
        bool m_mute = false;
        const double m_clickGain = 1.0;
        const double m_blockToneGain = 0.10;

        quint16 m_id;
        swift::misc::CSettingReadOnly<swift::misc::audio::TSettings> m_audioSettings { this };

        swift::sound::sample_provider::CVolumeSampleProvider *m_volume = nullptr;
        swift::sound::sample_provider::CMixingSampleProvider *m_mixer = nullptr;
        swift::sound::sample_provider::CSinusGenerator *m_blockTone = nullptr;
        QVector<CCallsignSampleProvider *> m_voiceInputs;
        qint64 m_lastLogMessage = -1;

        QString m_receivingCallsignsString;
        swift::misc::aviation::CCallsignSet m_receivingCallsigns;

        bool m_doClickWhenAppropriate = false;
        bool m_doBlockWhenAppropriate = false;
        int m_lastNumberOfInUseInputs = 0;
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs)

#endif // guard
