/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "soundcardsampleprovider.h"

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            SoundcardSampleProvider::SoundcardSampleProvider(int sampleRate, const QVector<quint16> &transceiverIDs, QObject *parent) :
                ISampleProvider(parent),
                m_mixer(new MixingSampleProvider())
            {
                m_waveFormat.setSampleRate(sampleRate);
                m_waveFormat.setChannelCount(1);
                m_waveFormat.setSampleSize(16);
                m_waveFormat.setSampleType(QAudioFormat::SignedInt);
                m_waveFormat.setByteOrder(QAudioFormat::LittleEndian);
                m_waveFormat.setCodec("audio/pcm");

                m_mixer = new MixingSampleProvider(this);

                m_receiverIDs = transceiverIDs;

                for (quint16 transceiverID : transceiverIDs)
                {
                    ReceiverSampleProvider *transceiverInput = new ReceiverSampleProvider(m_waveFormat, transceiverID, 4, m_mixer);
                    connect(transceiverInput, &ReceiverSampleProvider::receivingCallsignsChanged, this, &SoundcardSampleProvider::receivingCallsignsChanged);
                    m_receiverInputs.push_back(transceiverInput);
                    m_receiverIDs.push_back(transceiverID);
                    m_mixer->addMixerInput(transceiverInput);
                }
            }

            QAudioFormat SoundcardSampleProvider::waveFormat() const
            {
                return m_waveFormat;
            }

            void SoundcardSampleProvider::setBypassEffects(bool value)
            {
                for (ReceiverSampleProvider *receiverInput : m_receiverInputs)
                {
                    receiverInput->setBypassEffects(value);
                }
            }

            void SoundcardSampleProvider::pttUpdate(bool active, const QVector<TxTransceiverDto> &txTransceivers)
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
                            auto it = std::find_if(m_receiverInputs.begin(), m_receiverInputs.end(), [txTransceiver](const ReceiverSampleProvider * p)
                            {
                                return p->getId() == txTransceiver.id;
                            });

                            if (it != m_receiverInputs.end()) { (*it)->setMute(true); }
                        }
                    }
                }
                else
                {
                    for (ReceiverSampleProvider *receiverInput : m_receiverInputs)
                    {
                        receiverInput->setMute(false);
                    }
                }
            }

            int SoundcardSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
            {
                return m_mixer->readSamples(samples, count);
            }

            void SoundcardSampleProvider::addOpusSamples(const IAudioDto &audioDto, const QVector<RxTransceiverDto> &rxTransceivers)
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
                        RxTransceiverDto rxTransceiver = rxTransceiversFilteredAndSorted[i];
                        if (!handledTransceiverIDs.contains(rxTransceiver.id))
                        {
                            handledTransceiverIDs.push_back(rxTransceiver.id);

                            ReceiverSampleProvider *receiverInput = nullptr;
                            auto it = std::find_if(m_receiverInputs.begin(), m_receiverInputs.end(), [rxTransceiver](const ReceiverSampleProvider * p)
                            {
                                return p->getId() == rxTransceiver.id;
                            });
                            if (it != m_receiverInputs.end())
                            {
                                receiverInput = *it;
                            }

                            if (! receiverInput) { continue; }
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

            void SoundcardSampleProvider::updateRadioTransceivers(const QVector<TransceiverDto> &radioTransceivers)
            {
                for (const TransceiverDto &radioTransceiver : radioTransceivers)
                {
                    auto it = std::find_if(m_receiverInputs.begin(), m_receiverInputs.end(), [radioTransceiver](const ReceiverSampleProvider * p)
                    {
                        return p->getId() == radioTransceiver.id;
                    });

                    if (it)
                    {
                        (*it)->setFrequency(radioTransceiver.frequency);
                    }
                }

                for (ReceiverSampleProvider *receiverInput : m_receiverInputs)
                {
                    quint16 transceiverID = receiverInput->getId();
                    bool contains = std::any_of(radioTransceivers.begin(), radioTransceivers.end(), [&](const auto & tx) { return transceiverID == tx.id; });
                    if (! contains)
                    {
                        receiverInput->setFrequency(0);
                    }
                }
            }

            QString SoundcardSampleProvider::getReceivingCallsigns(quint16 transceiverID)
            {
                return m_receiverInputs.at(transceiverID)->getReceivingCallsigns();
            }

        } // ns
    } // ns
} // ns
