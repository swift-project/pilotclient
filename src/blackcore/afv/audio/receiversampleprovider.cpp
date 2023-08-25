// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackcore/afv/audio/receiversampleprovider.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/metadatautils.h"
#include "blacksound/sampleprovider/resourcesoundsampleprovider.h"
#include "blacksound/sampleprovider/samples.h"

#include <QDebug>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackSound::SampleProvider;

namespace BlackCore::Afv::Audio
{
    const QStringList &CReceiverSampleProvider::getLogCategories()
    {
        static const QStringList cats { CLogCategories::audio(), CLogCategories::vatsimSpecific() };
        return cats;
    }

    CReceiverSampleProvider::CReceiverSampleProvider(const QAudioFormat &audioFormat, quint16 id, int voiceInputNumber, QObject *parent) : ISampleProvider(parent),
                                                                                                                                           m_id(id)
    {
        const QString on = QStringLiteral("%1 id: %2").arg(classNameShort(this)).arg(id);
        this->setObjectName(on);

        m_mixer = new CMixingSampleProvider(this);
        for (int i = 0; i < voiceInputNumber; i++)
        {
            const auto voiceInput = new CCallsignSampleProvider(audioFormat, this, m_mixer);
            m_voiceInputs.push_back(voiceInput);
            m_mixer->addMixerInput(voiceInput);
        }

        m_blockTone = new CSinusGenerator(180, this);
        m_mixer->addMixerInput(m_blockTone);
        m_volume = new CVolumeSampleProvider(m_mixer);
    }

    void CReceiverSampleProvider::setBypassEffects(bool value)
    {
        for (CCallsignSampleProvider *voiceInput : std::as_const(m_voiceInputs))
        {
            voiceInput->setBypassEffects(value);
        }
    }

    void CReceiverSampleProvider::setFrequency(const uint &frequencyHz)
    {
        if (frequencyHz != m_frequencyHz)
        {
            for (CCallsignSampleProvider *voiceInput : std::as_const(m_voiceInputs))
            {
                voiceInput->clear();
            }
        }
        m_frequencyHz = frequencyHz;
    }

    int CReceiverSampleProvider::activeCallsigns() const
    {
        const int numberOfCallsigns = static_cast<int>(std::count_if(m_voiceInputs.begin(), m_voiceInputs.end(), [](const CCallsignSampleProvider *p) {
            return p->inUse() == true;
        }));
        return numberOfCallsigns;
    }

    void CReceiverSampleProvider::setMute(bool value)
    {
        m_mute = value;
        if (value)
        {
            for (CCallsignSampleProvider *voiceInput : std::as_const(m_voiceInputs))
            {
                voiceInput->clear();
            }
        }
    }

    int CReceiverSampleProvider::readSamples(QVector<float> &samples, qint64 count)
    {
        int numberOfInUseInputs = activeCallsigns();
        if (numberOfInUseInputs > 1 && m_doBlockWhenAppropriate)
        {
            m_blockTone->setFrequency(180.0);
            m_blockTone->setGain(m_blockToneGain);
        }
        else
        {
            m_blockTone->setGain(0.0);
        }

        if (m_doClickWhenAppropriate && numberOfInUseInputs == 0)
        {
            CResourceSoundSampleProvider *resourceSound = new CResourceSoundSampleProvider(Samples::instance().click(), m_mixer);
            m_mixer->addMixerInput(resourceSound);
            m_doClickWhenAppropriate = false;
            // CLogMessage(this).debug(u"AFV Click...");
        }

        //! \todo KB 2020-04 not entirely correct, as it can be the number is the same, but changed callsign
        if (numberOfInUseInputs != m_lastNumberOfInUseInputs)
        {
            QStringList receivingCallsigns;
            for (const CCallsignSampleProvider *voiceInput : std::as_const(m_voiceInputs))
            {
                const QString callsign = voiceInput->callsign();
                if (!callsign.isEmpty())
                {
                    receivingCallsigns.push_back(callsign);
                }
            }

            m_receivingCallsignsString = receivingCallsigns.join(',');
            m_receivingCallsigns = CCallsignSet(receivingCallsigns);
            const TransceiverReceivingCallsignsChangedArgs args = { m_id, receivingCallsigns };
            emit receivingCallsignsChanged(args);
        }
        m_lastNumberOfInUseInputs = numberOfInUseInputs;
        return m_volume->readSamples(samples, count);
    }

    void CReceiverSampleProvider::addOpusSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio)
    {
        if (m_frequencyHz != frequency) { return; } // Lag in the backend means we get the tail end of a transmission
        CCallsignSampleProvider *voiceInput = nullptr;

        auto it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [audioDto](const CCallsignSampleProvider *p) {
            return p->callsign() == audioDto.callsign;
        });

        if (it != m_voiceInputs.end())
        {
            voiceInput = *it;
        }

        if (!voiceInput)
        {
            it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [](const CCallsignSampleProvider *p) { return p->inUse() == false; });
            if (it != m_voiceInputs.end())
            {
                voiceInput = *it;
                voiceInput->active(audioDto.callsign, "");
            }
        }

        if (voiceInput)
        {
            voiceInput->addOpusSamples(audioDto, distanceRatio);
        }

        const CSettings s = m_audioSettings.get();
        m_doClickWhenAppropriate = s.afvClicked();
        m_doBlockWhenAppropriate = s.afvBlocked();
    }

    void CReceiverSampleProvider::addSilentSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio)
    {
        Q_UNUSED(distanceRatio)
        if (m_frequencyHz != frequency) { return; } // Lag in the backend means we get the tail end of a transmission

        CCallsignSampleProvider *voiceInput = nullptr;
        auto it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [audioDto](const CCallsignSampleProvider *p) {
            return p->callsign() == audioDto.callsign;
        });

        if (it != m_voiceInputs.end())
        {
            voiceInput = *it;
        }

        if (!voiceInput)
        {
            it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [](const CCallsignSampleProvider *p) { return p->inUse() == false; });
            if (it != m_voiceInputs.end())
            {
                voiceInput = *it;
                voiceInput->active(audioDto.callsign, "");
            }
        }

        if (voiceInput)
        {
            voiceInput->addSilentSamples(audioDto);
        }
    }

    uint CReceiverSampleProvider::getFrequencyHz() const
    {
        return m_frequencyHz;
    }

    void CReceiverSampleProvider::logVoiceInputs(const QString &prefix, qint64 timeCheckOffsetMs)
    {
        if (timeCheckOffsetMs > 100)
        {
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (m_lastLogMessage + timeCheckOffsetMs > now) { return; }
            m_lastLogMessage = now;
        }

        QString l;
        int no = 0;
        for (const CCallsignSampleProvider *sp : std::as_const(m_voiceInputs))
        {
            if (!sp || !sp->inUse()) { continue; } // only log the ones in use
            l += (l.isEmpty() ? QStringLiteral("") : QStringLiteral("\n")) %
                 prefix %
                 QString::number(no++) % QStringLiteral(": ") % sp->toQString();
        }

        if (l.isEmpty()) { return; }
        CLogMessage(this).debug(l);
    }

} // ns
