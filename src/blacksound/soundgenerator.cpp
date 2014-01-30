#include "soundgenerator.h"
#include <math.h>
#include <qmath.h>
#include <qendian.h>
#include <QAudioOutput>
#include <QtConcurrent/QtConcurrent>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Voice;

namespace BlackSound
{
    CSoundGenerator::CSoundGenerator(const QAudioDeviceInfo &device, const QAudioFormat &format, const QList<Tone> &tones, PlayMode mode, QObject *parent)
        :   QIODevice(parent),
            m_tones(tones), m_position(0), m_playMode(mode), m_endReached(false), m_oneCycleDurationMs(calculateDurationMs(tones)),
            m_device(device), m_audioFormat(format), m_audioOutput(new QAudioOutput(format))
    {
        Q_ASSERT(tones.size() > 0);
    }

    CSoundGenerator::CSoundGenerator(const QList<Tone> &tones, PlayMode mode, QObject *parent)
        :   QIODevice(parent),
            m_tones(tones), m_position(0), m_playMode(mode), m_endReached(false), m_oneCycleDurationMs(calculateDurationMs(tones)),
            m_device(QAudioDeviceInfo::defaultOutputDevice()), m_audioFormat(CSoundGenerator::defaultAudioFormat()),
            m_audioOutput(new QAudioOutput(CSoundGenerator::defaultAudioFormat()))
    {
        Q_ASSERT(tones.size() > 0);
    }

    CSoundGenerator::~CSoundGenerator()
    {
        this->stop(true);
    }

    void CSoundGenerator::start(int volume)
    {
        if (volume < 1) return;
        if (this->m_buffer.isEmpty()) this->generateData();
        this->open(QIODevice::ReadOnly);
        this->m_audioOutput->setVolume(qreal(0.01 * volume));
        this->m_audioOutput->start(this); // pull
    }

    void CSoundGenerator::stop(bool destructor)
    {
        this->m_audioOutput->setVolume(0);
        this->m_audioOutput->stop();
        if (this->isOpen())
        {
            // 1. isOpen avoids redundant signals
            // 2. OK in destructor, see http://stackoverflow.com/a/14024955/356726
            emit this->stopped();
            this->close(); // close IO Device
        }
        this->m_position = 0;
        if (destructor) return;

        // trigger own termination
        if (this->m_playMode == SingleWithAutomaticDeletion) this->deleteLater();
    }

    void CSoundGenerator::generateData()
    {
        Q_ASSERT(this->m_tones.size() > 0);
        const int bytesPerSample = this->m_audioFormat.sampleSize() / 8;
        const int bytesForAllChannels = this->m_audioFormat.channelCount() * bytesPerSample;

        qint64 totalLength = 0;
        foreach(Tone t, this->m_tones)
        {
            totalLength += this->m_audioFormat.sampleRate() * bytesForAllChannels * t.m_durationMs / 1000;
        }

        Q_ASSERT(totalLength % bytesForAllChannels == 0);
        Q_UNUSED(bytesForAllChannels) // suppress warning in release builds

        m_buffer.resize(totalLength);
        unsigned char *bufferPointer = reinterpret_cast<unsigned char *>(m_buffer.data());

        foreach(Tone t, this->m_tones)
        {
            qint64 lengthPerTone = this->m_audioFormat.sampleRate() * bytesForAllChannels * t.m_durationMs / 1000;
            int sampleIndexPerTone = 0;

            while (lengthPerTone)
            {
                // http://hyperphysics.phy-astr.gsu.edu/hbase/audio/sumdif.html
                // http://math.stackexchange.com/questions/164369/how-do-you-calculate-the-frequency-perceived-by-humans-of-two-sinusoidal-waves-a
                const double pseudoTime = double(sampleIndexPerTone % this->m_audioFormat.sampleRate()) / this->m_audioFormat.sampleRate();
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

                for (int i = 0; i < this->m_audioFormat.channelCount(); ++i)
                {
                    if (this->m_audioFormat.sampleSize() == 8 && this->m_audioFormat.sampleType() == QAudioFormat::UnSignedInt)
                    {
                        const quint8 value = static_cast<quint8>((1.0 + amplitude) / 2 * 255);
                        *reinterpret_cast<quint8 *>(bufferPointer) = value;
                    }
                    else if (this->m_audioFormat.sampleSize() == 8 && this->m_audioFormat.sampleType() == QAudioFormat::SignedInt)
                    {
                        const qint8 value = static_cast<qint8>(amplitude * 127);
                        *reinterpret_cast<quint8 *>(bufferPointer) = value;
                    }
                    else if (this->m_audioFormat.sampleSize() == 16 && this->m_audioFormat.sampleType() == QAudioFormat::UnSignedInt)
                    {
                        quint16 value = static_cast<quint16>((1.0 + amplitude) / 2 * 65535);
                        if (this->m_audioFormat.byteOrder() == QAudioFormat::LittleEndian)
                            qToLittleEndian<quint16>(value, bufferPointer);
                        else
                            qToBigEndian<quint16>(value, bufferPointer);
                    }
                    else if (this->m_audioFormat.sampleSize() == 16 && this->m_audioFormat.sampleType() == QAudioFormat::SignedInt)
                    {
                        qint16 value = static_cast<qint16>(amplitude * 32767);
                        if (this->m_audioFormat.byteOrder() == QAudioFormat::LittleEndian)
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
        if (len < 1) return 0;
        if (this->m_endReached)
        {
            this->stop(); // all data read, we can stop output
            return 0;
        }
        if (!this->isOpen()) return 0;
        qint64 total = 0; // toal is used for the overflow when starting new wave again
        while (len - total > 0)
        {
            const qint64 chunkSize = qMin((m_buffer.size() - m_position), (len - total));
            memcpy(data + total, m_buffer.constData() + m_position, chunkSize);
            this->m_position = (m_position + chunkSize) % m_buffer.size();
            total += chunkSize;
            if (m_position == 0 &&
                    (m_playMode == Single || m_playMode == SingleWithAutomaticDeletion))
            {
                this->m_endReached = true;
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

    /*
     * BlackMisc to Qt audio device
     */
    QAudioDeviceInfo CSoundGenerator::findClosestOutputDevice(const BlackMisc::Voice::CAudioDevice &audioDevice)
    {
        Q_ASSERT(audioDevice.getType() == CAudioDevice::OutputDevice);
        const QString lookFor = audioDevice.getName().toLower();
        QAudioDeviceInfo qtDevice = QAudioDeviceInfo::defaultOutputDevice();
        if (lookFor.startsWith("default")) return qtDevice;
        int score = 0;
        foreach(QAudioDeviceInfo qd, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        {
            const QString cn = qd.deviceName().toLower();
            if (lookFor == cn) return qd; // exact match
            if (cn.length() < lookFor.length())
            {
                if (lookFor.contains(cn) && cn.length() > score)
                {
                    qtDevice = qd;
                    score = cn.length();
                }
            }
            else
            {
                if (cn.contains(lookFor) && lookFor.length() > score)
                {
                    qtDevice = qd;
                    score = lookFor.length();
                }
            }
        }
        return qtDevice;

    }

    void CSoundGenerator::playSignal(qint32 volume, const QList<CSoundGenerator::Tone> &tones, QAudioDeviceInfo device)
    {
        if (tones.isEmpty()) return; // that was easy
        if (volume < 1) return;
        CSoundGenerator *generator = new CSoundGenerator(device, CSoundGenerator::defaultAudioFormat(), tones, CSoundGenerator::SingleWithAutomaticDeletion);
        if (generator->singleCyleDurationMs() < 10) return; // unable to hear

        // top and clean uo when done
        generator->start(volume);
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

    void CSoundGenerator::playSelcal(qint32 volume, const CSelcal &selcal, const CAudioDevice &audioDevice)
    {
        CSoundGenerator::playSelcal(volume, selcal, CSoundGenerator::findClosestOutputDevice(audioDevice));
    }

} // namespace
