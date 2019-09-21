/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "receiversampleprovider.h"
#include "blacksound/sampleprovider/resourcesoundsampleprovider.h"
#include "blacksound/sampleprovider/samples.h"

#include <QDebug>

using namespace BlackSound::SampleProvider;

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            CReceiverSampleProvider::CReceiverSampleProvider(const QAudioFormat &audioFormat, quint16 id, int voiceInputNumber, QObject *parent) :
                ISampleProvider(parent),
                m_id(id)
            {
                m_mixer = new CMixingSampleProvider(this);

                for (int i = 0; i < voiceInputNumber; i++)
                {
                    auto voiceInput = new CallsignSampleProvider(audioFormat, m_mixer);
                    m_voiceInputs.push_back(voiceInput);
                    m_mixer->addMixerInput(voiceInput);
                }

                // TODO blockTone = new SignalGenerator(WaveFormat.SampleRate, 1) { Gain = 0, Type = SignalGeneratorType.Sin, Frequency = 180 };
                // TODO mixer.AddMixerInput(blockTone.ToMono());
                // TODO volume = new VolumeSampleProvider(mixer);
            }

            void CReceiverSampleProvider::setBypassEffects(bool value)
            {
                for (CallsignSampleProvider *voiceInput : m_voiceInputs)
                {
                    voiceInput->setBypassEffects(value);
                }
            }

            void CReceiverSampleProvider::setFrequency(const uint &frequency)
            {
                if (frequency != m_frequency)
                {
                    for (CallsignSampleProvider *voiceInput : m_voiceInputs)
                    {
                        voiceInput->clear();
                    }
                }
                m_frequency = frequency;
            }

            int CReceiverSampleProvider::activeCallsigns() const
            {
                const int numberOfCallsigns = std::count_if(m_voiceInputs.begin(), m_voiceInputs.end(), [](const CallsignSampleProvider * p)
                {
                    return p->inUse() == true;
                });
                return numberOfCallsigns;
            }

            float CReceiverSampleProvider::volume() const
            {
                return 1.0;
            }

            bool CReceiverSampleProvider::getMute() const
            {
                return m_mute;
            }

            void CReceiverSampleProvider::setMute(bool value)
            {
                m_mute = value;
                if (value)
                {
                    for (CallsignSampleProvider *voiceInput : m_voiceInputs)
                    {
                        voiceInput->clear();
                    }
                }
            }

            int CReceiverSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
            {
                int numberOfInUseInputs = activeCallsigns();

                if (numberOfInUseInputs > 1)
                {
//        blockTone.Frequency = 180;
//        blockTone.Gain = blockToneGain;
                }
                else
                {
//        blockTone.Gain = 0;
                }

                if (m_doClickWhenAppropriate && numberOfInUseInputs == 0)
                {
                    CResourceSoundSampleProvider *resourceSound = new CResourceSoundSampleProvider(Samples::instance().click(), m_mixer);
                    m_mixer->addMixerInput(resourceSound);
                    qDebug() << "Click...";
                    m_doClickWhenAppropriate = false;
                }

                if (numberOfInUseInputs != lastNumberOfInUseInputs)
                {
                    QStringList receivingCallsigns;
                    for (const CallsignSampleProvider *voiceInput : m_voiceInputs)
                    {
                        QString callsign = voiceInput->callsign();
                        if (! callsign.isEmpty())
                        {
                            receivingCallsigns.push_back(callsign);
                        }
                    }

                    m_receivingCallsigns = receivingCallsigns.join(',');
                    TransceiverReceivingCallsignsChangedArgs args = { m_id, receivingCallsigns };
                    emit receivingCallsignsChanged(args);
                }
                lastNumberOfInUseInputs = numberOfInUseInputs;

//            return volume.Read(buffer, offset, count);
                return m_mixer->readSamples(samples, count);
            }

            void CReceiverSampleProvider::addOpusSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio)
            {
                if (m_frequency != frequency)        //Lag in the backend means we get the tail end of a transmission
                    return;

                CallsignSampleProvider *voiceInput = nullptr;

                auto it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [audioDto](const CallsignSampleProvider * p)
                {
                    return p->callsign() == audioDto.callsign;
                });
                if (it != m_voiceInputs.end())
                {
                    voiceInput = *it;
                }

                if (! voiceInput)
                {
                    it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [](const CallsignSampleProvider * p) { return p->inUse() == false; });
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

                m_doClickWhenAppropriate = true;
            }

            void CReceiverSampleProvider::addSilentSamples(const IAudioDto &audioDto, uint frequency, float distanceRatio)
            {
                Q_UNUSED(distanceRatio);
                if (m_frequency != frequency)        //Lag in the backend means we get the tail end of a transmission
                    return;

                CallsignSampleProvider *voiceInput = nullptr;

                auto it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [audioDto](const CallsignSampleProvider * p)
                {
                    return p->callsign() == audioDto.callsign;
                });
                if (it != m_voiceInputs.end())
                {
                    voiceInput = *it;
                }

                if (! voiceInput)
                {
                    it = std::find_if(m_voiceInputs.begin(), m_voiceInputs.end(), [](const CallsignSampleProvider * p) { return p->inUse() == false; });
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

            QString CReceiverSampleProvider::getReceivingCallsigns() const
            {
                return m_receivingCallsigns;
            }


        } // ns
    } // ns
} // ns
