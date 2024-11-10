// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/afv/audio/callsignsampleprovider.h"
#include "blackcore/afv/audio/callsigndelaycache.h"

#include "blackcore/afv/audio/receiversampleprovider.h"
#include "sound/sampleprovider/samples.h"
#include "sound/audioutilities.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/metadatautils.h"
#include "config/buildconfig.h"

#include <QtMath>
#include <QDebug>
#include <QStringLiteral>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace swift::sound::sample_provider;
using namespace swift::config;

namespace BlackCore::Afv::Audio
{
    CCallsignSampleProvider::CCallsignSampleProvider(const QAudioFormat &audioFormat, const CReceiverSampleProvider *receiver, QObject *parent) : ISampleProvider(parent),
                                                                                                                                                  m_audioFormat(audioFormat),
                                                                                                                                                  m_receiver(receiver),
                                                                                                                                                  m_decoder(audioFormat.sampleRate(), 1)
    {
        Q_ASSERT(audioFormat.channelCount() == 1);
        Q_ASSERT(receiver);

        const QString on = QStringLiteral("%1").arg(classNameShort(this));
        this->setObjectName(on);

        m_mixer = new CMixingSampleProvider(this);
        m_crackleSoundProvider = new CResourceSoundSampleProvider(Samples::instance().crackle(), m_mixer);
        m_crackleSoundProvider->setLooping(true);
        m_crackleSoundProvider->setGain(0.0);
        m_whiteNoise = new CResourceSoundSampleProvider(Samples::instance().whiteNoise(), m_mixer);
        m_whiteNoise->setLooping(true);
        m_whiteNoise->setGain(0.0);
        m_hfWhiteNoise = new CResourceSoundSampleProvider(Samples::instance().hfWhiteNoise(), m_mixer);
        m_hfWhiteNoise->setLooping(true);
        m_hfWhiteNoise->setGain(0.0);
        m_acBusNoise = new CSawToothGenerator(400, m_mixer);
        m_audioInput = new CBufferedWaveProvider(audioFormat, m_mixer);

        // Create the compressor
        m_simpleCompressorEffect = new CSimpleCompressorEffect(m_audioInput, m_mixer);
        m_simpleCompressorEffect->setMakeUpGain(-5.5);

        // Create the voice EQ
        m_voiceEqualizer = new CEqualizerSampleProvider(m_simpleCompressorEffect, EqualizerPresets::VHFEmulation, m_mixer);

        m_mixer->addMixerInput(m_whiteNoise);
        m_mixer->addMixerInput(m_acBusNoise);
        m_mixer->addMixerInput(m_hfWhiteNoise);
        m_mixer->addMixerInput(m_voiceEqualizer);

        m_timer = new QTimer(this);
        m_timer->setObjectName(this->objectName() + ":m_timer");

        m_timer->setInterval(100);
        connect(m_timer, &QTimer::timeout, this, &CCallsignSampleProvider::timerElapsed);
    }

    int CCallsignSampleProvider::readSamples(QVector<float> &samples, qint64 count)
    {
        const int noOfSamples = m_mixer->readSamples(samples, count);

        if (m_inUse && m_lastPacketLatch && m_audioInput->getBufferedBytes() == 0)
        {
            idle();
            m_lastPacketLatch = false;
        }

        if (m_inUse && !m_underflow && m_audioInput->getBufferedBytes() == 0)
        {
            if (verbose()) { CLogMessage(this).debug(u"[%1] [Delay++]") << m_callsign; }
            CallsignDelayCache::instance().underflow(m_callsign);
            m_underflow = true;
        }

        return noOfSamples;
    }

    void CCallsignSampleProvider::timerElapsed()
    {
        if (m_inUse && m_audioInput->getBufferedBytes() == 0 && m_lastSamplesAddedUtc.msecsTo(QDateTime::currentDateTimeUtc()) > m_idleTimeoutMs)
        {
            idle();
        }
    }

    void CCallsignSampleProvider::active(const QString &callsign, const QString &aircraftType)
    {
        m_callsign = callsign;
        CallsignDelayCache::instance().initialise(callsign);
        m_aircraftType = aircraftType;
        m_decoder.resetState();
        m_inUse = true;
        setEffects();
        m_underflow = false;

        const int delayMs = CallsignDelayCache::instance().get(callsign);
        if (verbose()) { CLogMessage(this).debug(u"[%1] [Delay %2ms]") << m_callsign << delayMs; }
        if (delayMs > 0)
        {
            const int phaseDelayLength = (m_audioFormat.sampleRate() / 1000) * delayMs;
            const QVector<float> phaseDelay(phaseDelayLength * 2, 0);
            m_audioInput->addSamples(phaseDelay);
        }
    }

    void CCallsignSampleProvider::activeSilent(const QString &callsign, const QString &aircraftType)
    {
        m_callsign = callsign;
        CallsignDelayCache::instance().initialise(callsign);
        m_aircraftType = aircraftType;
        m_decoder.resetState();
        m_inUse = true;
        setEffects(true);
        m_underflow = true;
    }

    void CCallsignSampleProvider::clear()
    {
        idle();
        m_audioInput->clearBuffer();
    }

    void CCallsignSampleProvider::addOpusSamples(const IAudioDto &audioDto, float distanceRatio)
    {
        m_distanceRatio = distanceRatio;
        setEffects();

        const QVector<qint16> audio = decodeOpus(audioDto.audio);
        m_audioInput->addSamples(swift::sound::convertFromShortToFloat(audio));
        m_lastPacketLatch = audioDto.lastPacket;
        if (audioDto.lastPacket && !m_underflow) { CallsignDelayCache::instance().success(m_callsign); }
        m_lastSamplesAddedUtc = QDateTime::currentDateTimeUtc();
        if (!m_timer->isActive()) { m_timer->start(); }
    }

    void CCallsignSampleProvider::addSilentSamples(const IAudioDto &audioDto)
    {
        // Disable all audio effects
        setEffects(true);

        // TODO audioInput->addSamples(decoderByteBuffer, 0, frameCount * 2);
        m_lastPacketLatch = audioDto.lastPacket;

        m_lastSamplesAddedUtc = QDateTime::currentDateTimeUtc();
        if (!m_timer->isActive()) { m_timer->start(); }
    }

    void CCallsignSampleProvider::idle()
    {
        m_timer->stop();
        m_inUse = false;
        setEffects();
        m_callsign.clear();
        m_aircraftType.clear();
    }

    QVector<qint16> CCallsignSampleProvider::decodeOpus(const QByteArray &opusData)
    {
        int decodedLength = 0;
        const QVector<qint16> decoded = m_decoder.decode(opusData, opusData.size(), &decodedLength);
        return decoded;
    }

    void CCallsignSampleProvider::setEffects(bool noEffects)
    {
        if (noEffects || m_bypassEffects || !m_inUse)
        {
            m_crackleSoundProvider->setGain(0.0);
            m_whiteNoise->setGain(0.0);
            m_hfWhiteNoise->setGain(0.0);
            m_acBusNoise->setGain(0.0);
            m_simpleCompressorEffect->setEnabled(false);
            m_voiceEqualizer->setBypassEffects(true);
        }
        else
        {
            if (m_receiver->getFrequencyHz() < 30000000)
            {
                /**
                double crackleFactor = (((qExp(m_distanceRatio) * qPow(m_distanceRatio, -4.0)) / 350.0) - 0.00776652);
                if (crackleFactor < 0.0f)  { crackleFactor = 0.00f; }
                if (crackleFactor > 0.20f) { crackleFactor = 0.20f; }
                **/

                m_hfWhiteNoise->setGain(m_hfWhiteNoiseGainMin);
                m_acBusNoise->setGain(m_acBusGainMin + 0.001f);
                m_simpleCompressorEffect->setEnabled(true);
                m_voiceEqualizer->setBypassEffects(false);
                m_voiceEqualizer->setOutputGain(0.38);
                m_whiteNoise->setGain(0.0);
            }
            else
            {
                double crackleFactor = (((qExp(m_distanceRatio) * qPow(m_distanceRatio, -4.0)) / 350.0) - 0.00776652);

                if (crackleFactor < 0.0) { crackleFactor = 0.0; }
                if (crackleFactor > 0.20) { crackleFactor = 0.20; }

                m_crackleSoundProvider->setGain(crackleFactor * 2);
                m_whiteNoise->setGain(m_whiteNoiseGainMin);
                m_acBusNoise->setGain(m_acBusGainMin);
                m_simpleCompressorEffect->setEnabled(true);
                m_voiceEqualizer->setBypassEffects(false);
                m_voiceEqualizer->setOutputGain(1.0 - crackleFactor * 3.7);
            }
        }
    }

    void CCallsignSampleProvider::setBypassEffects(bool bypassEffects)
    {
        m_bypassEffects = bypassEffects;
        setEffects();
    }

    QString CCallsignSampleProvider::toQString() const
    {
        return QStringLiteral("In use: ") % boolToYesNo(m_inUse) %
               QStringLiteral(" cs: ") % m_callsign %
               QStringLiteral(" type: ") % m_aircraftType;
    }

} // ns
