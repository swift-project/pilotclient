#include "soundgenerator.h"
#include <math.h>
#include <qmath.h>
#include <qendian.h>
#include <QAudioOutput>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSound
{
    CSoundGenerator::CSoundGenerator(const QAudioFormat &format, const QList<Tone> &tones, bool singlePlay, QObject *parent)
        :   QIODevice(parent), m_position(0), m_singlePlay(singlePlay), m_endReached(false), m_oneCycleDurationMs(calculateDurationMs(tones))
    {
        Q_ASSERT(tones.size() > 0);
        this->generateData(format, tones);
    }

    CSoundGenerator::CSoundGenerator(const QList<Tone> &tones, bool singlePlay, QObject *parent)
        :   QIODevice(parent), m_position(0), m_singlePlay(singlePlay), m_endReached(false), m_oneCycleDurationMs(calculateDurationMs(tones))
    {
        Q_ASSERT(tones.size() > 0);
        this->generateData(CSoundGenerator::defaultAudioFormat(), tones);
    }

    CSoundGenerator::~CSoundGenerator()
    {
        this->close();
    }

    void CSoundGenerator::start()
    {
        this->open(QIODevice::ReadOnly);
    }

    void CSoundGenerator::stop()
    {
        this->close();
        this->m_position = 0;
        emit this->stopped();
    }

    void CSoundGenerator::generateData(const QAudioFormat &format, const QList<Tone> &tones)
    {
        Q_ASSERT(tones.size() > 0);

        const int bytesPerSample = format.sampleSize() / 8;
        const int bytesForAllChannels = format.channelCount() * bytesPerSample;

        qint64 totalLength = 0;
        foreach(Tone t, tones)
        {
            totalLength += format.sampleRate() * bytesForAllChannels * t.m_durationMs / 1000;
        }

        Q_ASSERT(totalLength % bytesForAllChannels == 0);
        Q_UNUSED(bytesForAllChannels) // suppress warning in release builds

        m_buffer.resize(totalLength);
        unsigned char *bufferPointer = reinterpret_cast<unsigned char *>(m_buffer.data());

        foreach(Tone t, tones)
        {
            qint64 lengthPerTone = format.sampleRate() * bytesForAllChannels * t.m_durationMs / 1000;
            int sampleIndexPerTone = 0;

            while (lengthPerTone)
            {
                // http://hyperphysics.phy-astr.gsu.edu/hbase/audio/sumdif.html
                // http://math.stackexchange.com/questions/164369/how-do-you-calculate-the-frequency-perceived-by-humans-of-two-sinusoidal-waves-a
                const double pseudoTime = double(sampleIndexPerTone % format.sampleRate()) / format.sampleRate();
                double amplitude = 0; // silence
                if (t.m_frequencyHz > 10)
                {
                    amplitude = t.m_secondaryFrequencyHz == 0 ?
                                qSin(2 * M_PI * t.m_frequencyHz * pseudoTime) :
                                qSin(M_PI * (t.m_frequencyHz + t.m_secondaryFrequencyHz) * pseudoTime) *
                                qCos(M_PI * (t.m_frequencyHz - t.m_secondaryFrequencyHz) * pseudoTime);
                }

                // the combination of two frequencies actually would have 2*amplitude,
                // but I have to normalize with amplitude -1 -> +1

                for (int i = 0; i < format.channelCount(); ++i)
                {
                    if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt)
                    {
                        const quint8 value = static_cast<quint8>((1.0 + amplitude) / 2 * 255);
                        *reinterpret_cast<quint8 *>(bufferPointer) = value;
                    }
                    else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt)
                    {
                        const qint8 value = static_cast<qint8>(amplitude * 127);
                        *reinterpret_cast<quint8 *>(bufferPointer) = value;
                    }
                    else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt)
                    {
                        quint16 value = static_cast<quint16>((1.0 + amplitude) / 2 * 65535);
                        if (format.byteOrder() == QAudioFormat::LittleEndian)
                            qToLittleEndian<quint16>(value, bufferPointer);
                        else
                            qToBigEndian<quint16>(value, bufferPointer);
                    }
                    else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt)
                    {
                        qint16 value = static_cast<qint16>(amplitude * 32767);
                        if (format.byteOrder() == QAudioFormat::LittleEndian)
                            qToLittleEndian<qint16>(value, bufferPointer);
                        else
                            qToBigEndian<qint16>(value, bufferPointer);
                    }

                    bufferPointer += bytesPerSample;
                    lengthPerTone -= bytesPerSample;
                }
                ++sampleIndexPerTone;
            }
        }
    }

    qint64 CSoundGenerator::calculateDurationMs(const QList<CSoundGenerator::Tone> &tones)
    {
        if (tones.isEmpty()) return 0;
        qint64 d = 0;
        foreach(Tone t, tones)
        {
            d += t.m_durationMs;
        }
        return d;
    }

    qint64 CSoundGenerator::readData(char *data, qint64 len)
    {
        if (this->m_endReached) return 0;
        if (!this->isOpen()) return 0;
        qint64 total = 0;
        while (len - total > 0)
        {
            const qint64 chunk = qMin((m_buffer.size() - m_position), len - total);
            memcpy(data + total, m_buffer.constData() + m_position, chunk);
            this->m_position = (m_position + chunk) % m_buffer.size();
            total += chunk;
            if (m_singlePlay && m_position == 0)
            {
                this->m_endReached = true;
                this->stop();
                break;
            }
        }
        return total;
    }

    qint64 CSoundGenerator::writeData(const char *data, qint64 len)
    {
        Q_UNUSED(data);
        Q_UNUSED(len);

        return 0;
    }

    qint64 CSoundGenerator::bytesAvailable() const
    {
        return m_buffer.size() + QIODevice::bytesAvailable();
    }

    QAudioFormat CSoundGenerator::defaultAudioFormat()
    {
        QAudioFormat format;
        format.setSampleRate(44100);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);
        return format;
    }

    void CSoundGenerator::playSignal(qint32 volume, const QList<CSoundGenerator::Tone> &tones, QAudioDeviceInfo device)
    {
        if (tones.isEmpty()) return; // that was easy
        if (volume < 1) return;
        qint64 timeOut = calculateDurationMs(tones);
        if (timeOut < 10) return; // unable to hear
        QAudioOutput *audioOutput = new QAudioOutput(device, CSoundGenerator::defaultAudioFormat());
        CSoundGenerator *generator = new CSoundGenerator(tones, true, audioOutput);

        // top and clean uo when done
        connect(generator, &CSoundGenerator::stopped, audioOutput, &QAudioOutput::stop);
        connect(generator, &CSoundGenerator::stopped, audioOutput, &QAudioOutput::deleteLater);

        double vol = volume / 100.0;
        audioOutput->setVolume(vol);
        generator->start();
        audioOutput->start(generator);
    }

    void CSoundGenerator::playSelcal(qint32 volume, const BlackMisc::Aviation::CSelcal &selcal, QAudioDeviceInfo device)
    {
        if (volume < 1) return;
        if (!selcal.isValid()) return;
        QList<CFrequency> frequencies = selcal.getFrequencies();
        Q_ASSERT(frequencies.size() == 4);
        Tone t1(frequencies.at(0).value(CFrequencyUnit::Hz()), frequencies.at(1).value(CFrequencyUnit::Hz()), 1000);
        Tone t2(0, 200);
        Tone t3(frequencies.at(2).value(CFrequencyUnit::Hz()), frequencies.at(3).value(CFrequencyUnit::Hz()), 1000);
        QList<CSoundGenerator::Tone> tones;
        tones << t1 << t2 << t3;
        CSoundGenerator::playSignal(volume, tones, device);
    }

} // namespace
