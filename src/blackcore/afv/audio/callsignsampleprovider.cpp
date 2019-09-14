#include "callsignsampleprovider.h"
#include "callsigndelaycache.h"
#include "blacksound/sampleprovider/samples.h"
#include <QtMath>
#include <QDebug>

CallsignSampleProvider::CallsignSampleProvider(const QAudioFormat &audioFormat, QObject *parent) :
    ISampleProvider(parent),
    m_audioFormat(audioFormat),
    m_decoder(audioFormat.sampleRate(), 1)
{
    Q_ASSERT(audioFormat.channelCount() == 1);

    mixer = new MixingSampleProvider(this);
    crackleSoundProvider = new ResourceSoundSampleProvider(Samples::instance().crackle(), mixer);
    crackleSoundProvider->setLooping(true);
    crackleSoundProvider->setGain(0.0);
    whiteNoise = new ResourceSoundSampleProvider(Samples::instance().whiteNoise(), mixer);
    whiteNoise->setLooping(true);
    whiteNoise->setGain(0.0);
    acBusNoise = new SawToothGenerator(400, mixer);
    audioInput = new BufferedWaveProvider(audioFormat, mixer);

    // Create the compressor
    simpleCompressorEffect = new SimpleCompressorEffect(audioInput, mixer);
    simpleCompressorEffect->setMakeUpGain(-5.5);

    // Create the voice EQ
    voiceEq = new EqualizerSampleProvider(simpleCompressorEffect, EqualizerPresets::VHFEmulation, mixer);

    mixer->addMixerInput(whiteNoise);
    mixer->addMixerInput(acBusNoise);
    mixer->addMixerInput(voiceEq);

    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, &CallsignSampleProvider::timerElapsed);
}

int CallsignSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
{
    int noOfSamples = mixer->readSamples(samples, count);

    if (m_inUse && m_lastPacketLatch && audioInput->getBufferedBytes() == 0)
    {
        idle();
        m_lastPacketLatch = false;
    }

    if (m_inUse && !m_underflow && audioInput->getBufferedBytes() == 0)
    {
        qDebug() << "[" << m_callsign <<  "] [Delay++]";
        CallsignDelayCache::instance().underflow(m_callsign);
        m_underflow = true;
    }

    return noOfSamples;
}

void CallsignSampleProvider::timerElapsed()
{
    if (m_inUse && audioInput->getBufferedBytes() == 0 && m_lastSamplesAddedUtc.msecsTo(QDateTime::currentDateTimeUtc()) > idleTimeoutMs)
    {
        idle();
    }
}

QString CallsignSampleProvider::type() const
{
    return m_type;
}

void CallsignSampleProvider::active(const QString &callsign, const QString &aircraftType)
{
    m_callsign = callsign;
    CallsignDelayCache::instance().initialise(callsign);
    m_type = aircraftType;
    m_decoder.resetState();
    m_inUse = true;
    setEffects();
    m_underflow = false;

    int delayMs = CallsignDelayCache::instance().get(callsign);
    qDebug() << "[" << m_callsign << "] [Delay " << delayMs << "ms]";
    if (delayMs > 0)
    {
        int phaseDelayLength = (m_audioFormat.sampleRate() / 1000) * delayMs;
        QVector<qint16> phaseDelay(phaseDelayLength * 2, 0);
        audioInput->addSamples(phaseDelay);
    }
}

void CallsignSampleProvider::activeSilent(const QString &callsign, const QString &aircraftType)
{
    m_callsign = callsign;
    CallsignDelayCache::instance().initialise(callsign);
    m_type = aircraftType;
    m_decoder.resetState();
    m_inUse = true;
    setEffects(true);
    m_underflow = true;
}

void CallsignSampleProvider::clear()
{
    idle();
    audioInput->clearBuffer();
}

void CallsignSampleProvider::addOpusSamples(const IAudioDto &audioDto, float distanceRatio)
{
    m_distanceRatio = distanceRatio;

    QVector<qint16> audio = decodeOpus(audioDto.audio);
    audioInput->addSamples(audio);
    m_lastPacketLatch = audioDto.lastPacket;
    if (audioDto.lastPacket && !m_underflow)
        CallsignDelayCache::instance().success(m_callsign);

    m_lastSamplesAddedUtc = QDateTime::currentDateTimeUtc();
    if (!m_timer.isActive()) { m_timer.start(); }
}

void CallsignSampleProvider::addSilentSamples(const IAudioDto &audioDto)
{
    // Disable all audio effects
    setEffects(true);

    // TODO audioInput->addSamples(decoderByteBuffer, 0, frameCount * 2);
    m_lastPacketLatch = audioDto.lastPacket;

    m_lastSamplesAddedUtc = QDateTime::currentDateTimeUtc();
    if (!m_timer.isActive()) { m_timer.start(); }
}

QString CallsignSampleProvider::callsign() const
{
    return m_callsign;
}

void CallsignSampleProvider::idle()
{
    m_timer.stop();
    m_inUse = false;
    setEffects();
    m_callsign = QString();
    m_type = QString();
}

QVector<qint16> CallsignSampleProvider::decodeOpus(const QByteArray &opusData)
{
    int decodedLength = 0;
    QVector<qint16> decoded = m_decoder.decode(opusData, opusData.size(), &decodedLength);
    return decoded;
}

void CallsignSampleProvider::setEffects(bool noEffects)
{
    if (noEffects || m_bypassEffects || !m_inUse)
    {
        crackleSoundProvider->setGain(0.0);
        whiteNoise->setGain(0.0);
        acBusNoise->setGain(0.0);
        simpleCompressorEffect->setEnabled(false);
        voiceEq->setBypassEffects(true);
    }
    else
    {
        float crackleFactor = (float)(((qExp(m_distanceRatio) * qPow(m_distanceRatio, -4.0)) / 350) - 0.00776652);

        if (crackleFactor < 0.0f) { crackleFactor = 0.0f; }
        if (crackleFactor > 0.20f) { crackleFactor = 0.20f; }

        crackleSoundProvider->setGain(crackleFactor * 2);
        whiteNoise->setGain(whiteNoiseGainMin);
        acBusNoise->setGain(acBusGainMin);
        simpleCompressorEffect->setEnabled(true);
        voiceEq->setBypassEffects(false);
        voiceEq->setOutputGain(1.0 - crackleFactor * 3.7);
    }
}

void CallsignSampleProvider::setBypassEffects(bool bypassEffects)
{
    m_bypassEffects = bypassEffects;
    setEffects();
}

bool CallsignSampleProvider::inUse() const
{
    return m_inUse;
}
