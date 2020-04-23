/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "soundcardsampleprovider.h"
#include "blackmisc/metadatautils.h"

using namespace BlackMisc;
using namespace BlackSound::SampleProvider;

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            CSoundcardSampleProvider::CSoundcardSampleProvider(int sampleRate, const QVector<quint16> &transceiverIDs, QObject *parent) :
                ISampleProvider(parent),
                m_mixer(new CMixingSampleProvider())
            {
                const QString on = QStringLiteral("%1 sample rate: %2, transceivers: %3").arg(classNameShort(this)).arg(sampleRate).arg(transceiverIDs.size());
                this->setObjectName(on);

                m_waveFormat.setSampleRate(sampleRate);
                m_waveFormat.setChannelCount(1);
                m_waveFormat.setSampleSize(16);
                m_waveFormat.setSampleType(QAudioFormat::SignedInt);
                m_waveFormat.setByteOrder(QAudioFormat::LittleEndian);
                m_waveFormat.setCodec("audio/pcm");

                m_mixer = new CMixingSampleProvider(this);
                m_receiverIDs = transceiverIDs;

                constexpr int voiceInputNumber = 4; // number of CallsignSampleProviders
                for (quint16 transceiverID : transceiverIDs)
                {
                    CReceiverSampleProvider *transceiverInput = new CReceiverSampleProvider(m_waveFormat, transceiverID, voiceInputNumber, m_mixer);
                    connect(transceiverInput, &CReceiverSampleProvider::receivingCallsignsChanged, this, &CSoundcardSampleProvider::receivingCallsignsChanged);
                    m_receiverInputs.push_back(transceiverInput);
                    m_receiverIDs.push_back(transceiverID);
                    m_mixer->addMixerInput(transceiverInput);
                }
            }

            void CSoundcardSampleProvider::setBypassEffects(bool value)
            {
                for (CReceiverSampleProvider *receiverInput : m_receiverInputs)
                {
                    receiverInput->setBypassEffects(value);
                }
            }

            void CSoundcardSampleProvider::pttUpdate(bool active, const QVector<TxTransceiverDto> &txTransceivers)
            {
                if (active)
                {
                    if (txTransceivers.size() > 0)
                    {
                        QVector<TxTransceiverDto> txTransceiversFiltered = txTransceivers;

                        txTransceiversFiltered.erase(std::remove_if(txTransceiversFiltered.begin(), txTransceiversFiltered.end(), [this](const TxTransceiverDto & d)
                        {
                            return ! m_receiverIDs.contains(d.id);
                        }),
                        txTransceiversFiltered.end());


                        for (const TxTransceiverDto &txTransceiver : txTransceiversFiltered)
                        {
                            auto it = std::find_if(m_receiverInputs.begin(), m_receiverInputs.end(), [txTransceiver](const CReceiverSampleProvider * p)
                            {
                                return p->getId() == txTransceiver.id;
                            });

                            if (it != m_receiverInputs.end()) { (*it)->setMute(true); }
                        }
                    }
                }
                else
                {
                    for (CReceiverSampleProvider *receiverInput : m_receiverInputs)
                    {
                        receiverInput->setMute(false);
                    }
                }
            }

            int CSoundcardSampleProvider::readSamples(QVector<float> &samples, qint64 count)
            {
                return m_mixer->readSamples(samples, count);
            }

            void CSoundcardSampleProvider::addOpusSamples(const IAudioDto &audioDto, const QVector<RxTransceiverDto> &rxTransceivers)
            {
                QVector<RxTransceiverDto> rxTransceiversFilteredAndSorted = rxTransceivers;

                rxTransceiversFilteredAndSorted.erase(std::remove_if(rxTransceiversFilteredAndSorted.begin(), rxTransceiversFilteredAndSorted.end(), [this](const RxTransceiverDto & r)
                {
                    return !m_receiverIDs.contains(r.id);
                }),
                rxTransceiversFilteredAndSorted.end());

                std::sort(rxTransceiversFilteredAndSorted.begin(), rxTransceiversFilteredAndSorted.end(), [](const RxTransceiverDto & a, const RxTransceiverDto & b) -> bool
                {
                    return a.distanceRatio > b.distanceRatio;
                });

                if (rxTransceiversFilteredAndSorted.size() > 0)
                {
                    bool audioPlayed = false;
                    QVector<quint16> handledTransceiverIDs;
                    for (int i = 0; i < rxTransceiversFilteredAndSorted.size(); i++)
                    {
                        const RxTransceiverDto rxTransceiver = rxTransceiversFilteredAndSorted[i];
                        if (!handledTransceiverIDs.contains(rxTransceiver.id))
                        {
                            handledTransceiverIDs.push_back(rxTransceiver.id);

                            CReceiverSampleProvider *receiverInput = nullptr;
                            auto it = std::find_if(m_receiverInputs.begin(), m_receiverInputs.end(), [rxTransceiver](const CReceiverSampleProvider * p)
                            {
                                return p->getId() == rxTransceiver.id;
                            });

                            if (it != m_receiverInputs.end())
                            {
                                receiverInput = *it;
                            }

                            if (!receiverInput) { continue; }
                            if (receiverInput->getMute()) { continue; }

                            if (!audioPlayed)
                            {
                                receiverInput->addOpusSamples(audioDto, rxTransceiver.frequency, rxTransceiver.distanceRatio);
                                audioPlayed = true;
                            }
                            else
                            {
                                receiverInput->addSilentSamples(audioDto, rxTransceiver.frequency, rxTransceiver.distanceRatio);
                            }
                        }
                    }
                }
            }

            void CSoundcardSampleProvider::updateRadioTransceivers(const QVector<TransceiverDto> &radioTransceivers)
            {
                for (const TransceiverDto &radioTransceiver : radioTransceivers)
                {
                    auto it = std::find_if(m_receiverInputs.begin(), m_receiverInputs.end(), [radioTransceiver](const CReceiverSampleProvider * p)
                    {
                        return p->getId() == radioTransceiver.id;
                    });

                    if (it)
                    {
                        (*it)->setFrequency(radioTransceiver.frequencyHz);
                    }
                }

                for (CReceiverSampleProvider *receiverInput : m_receiverInputs)
                {
                    const quint16 transceiverID = receiverInput->getId();
                    const bool contains = std::any_of(radioTransceivers.cbegin(), radioTransceivers.cend(), [ transceiverID ](const auto &tx) { return transceiverID == tx.id; });
                    if (!contains)
                    {
                        receiverInput->setFrequency(0);
                    }
                }
            }

            QString CSoundcardSampleProvider::getReceivingCallsignsString(quint16 transceiverID) const
            {
                return m_receiverInputs.at(transceiverID)->getReceivingCallsignsString();
            }

            BlackMisc::Aviation::CCallsignSet CSoundcardSampleProvider::getReceivingCallsigns(quint16 transceiverID) const
            {
                return m_receiverInputs.at(transceiverID)->getReceivingCallsigns();
            }

        } // ns
    } // ns
} // ns
