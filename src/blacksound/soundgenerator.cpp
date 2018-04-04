/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blacksound/soundgenerator.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/filedeleter.h"
#include <QtCore/qendian.h>
#include <math.h>
#include <qmath.h>
#include <qendian.h>
#include <QMultimedia>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTimer>
#include <QUrl>
#include <QFile>
#include <QDir>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Audio;

namespace BlackSound
{
    QDateTime CSoundGenerator::s_selcalStarted  = QDateTime::currentDateTimeUtc();

    CSoundGenerator::CSoundGenerator(const QAudioDeviceInfo &device, const QAudioFormat &format, const QList<Tone> &tones, CNotificationSounds::PlayMode mode, QObject *parent)
        :   QIODevice(parent),
            m_tones(tones), m_position(0), m_playMode(mode), m_endReached(false), m_oneCycleDurationMs(calculateDurationMs(tones)),
            m_device(device), m_audioFormat(format), m_audioOutput(new QAudioOutput(format))
    {
        Q_ASSERT_X(tones.size() > 0, Q_FUNC_INFO, "No tones");
    }

    CSoundGenerator::CSoundGenerator(const QList<Tone> &tones, CNotificationSounds::PlayMode mode, QObject *parent)
        :   QIODevice(parent),
            m_tones(tones), m_position(0), m_playMode(mode), m_endReached(false), m_oneCycleDurationMs(calculateDurationMs(tones)),
            m_device(QAudioDeviceInfo::defaultOutputDevice()), m_audioFormat(CSoundGenerator::defaultAudioFormat()),
            m_audioOutput(new QAudioOutput(CSoundGenerator::defaultAudioFormat()))
    {
        Q_ASSERT_X(tones.size() > 0, Q_FUNC_INFO, "No tones");
    }

    CSoundGenerator::~CSoundGenerator()
    {
        this->stop(true);
        if (m_ownThread) { m_ownThread->deleteLater(); }
    }

    void CSoundGenerator::start(int volume, bool pull)
    {
        if (m_buffer.isEmpty()) this->generateData();
        this->open(QIODevice::ReadOnly);
        m_audioOutput->setVolume(qreal(0.01 * volume));

        if (pull)
        {
            // For an output device, the QAudioOutput class will pull data from the QIODevice
            // (using QIODevice::read()) when more audio data is required.
            m_audioOutput->start(this); // pull
        }
        else
        {
            // In push mode, the audio device provides a QIODevice instance that can be
            // written or read to as needed. Typically this results in simpler code but more buffering, which may affect latency.
            if (!m_pushTimer)
            {
                m_pushTimer = new QTimer(this);
                bool ok = connect(m_pushTimer, &QTimer::timeout, this, &CSoundGenerator::pushTimerExpired);
                Q_ASSERT(ok);
                Q_UNUSED(ok); // suppress Clang warning in release build
                m_pushTimer->start(20);
            }
            m_pushModeIODevice = m_audioOutput->start(); // push, IO device not owned
        }
    }

    void CSoundGenerator::startInOwnThread(int volume)
    {
        m_ownThread = new QThread(); // deleted by signals, hence no parent
        this->moveToThread(m_ownThread);
        // connect(this, &CSoundGenerator::startThread, this, &CSoundGenerator::start);

        connect(m_ownThread, &QThread::started, this, [ = ]() { this->start(volume, false); });
        connect(this, &CSoundGenerator::stopping, m_ownThread, &QThread::quit);

        // in auto delete mode force deleteLater when thread is finished
        if (m_playMode == CNotificationSounds::SingleWithAutomaticDeletion)
        {
            connect(m_ownThread, &QThread::finished, this, &CSoundGenerator::deleteLater);
        }

        // start thread and begin processing by calling start via signal startThread
        m_ownThread->start();
    }

    void CSoundGenerator::stop(bool destructor)
    {
        // m_audioOutput->setVolume(0); // Bug or feature, killing the applicaions volume?
        if (this->isOpen())
        {
            // 1. isOpen avoids redundant signals
            // 2. OK in destructor, see http://stackoverflow.com/a/14024955/356726
            this->close(); // close IO Device
            m_audioOutput->stop();
            if (m_pushTimer) { m_pushTimer->stop(); }
            emit this->stopped();
        }
        m_position = 0;
        if (destructor) return;

        // trigger own termination
        if (m_playMode == CNotificationSounds::SingleWithAutomaticDeletion)
        {
            emit this->stopping();
            if (!m_ownThread) this->deleteLater(); // with own thread, thread signal will call deleteLater
        }
    }

    void CSoundGenerator::pushTimerExpired()
    {
        if (m_pushModeIODevice && !m_endReached && m_audioOutput->state() != QAudio::StoppedState)
        {
            int chunks = m_audioOutput->bytesFree() / m_audioOutput->periodSize();
            while (chunks)
            {
                // periodSize-> Returns the period size in bytes.
                //! \todo looks wrong: read() will memcpy from m_buffer.constData() to m_buffer.data()
                const qint64 len = this->read(m_buffer.data(), m_audioOutput->periodSize());
                if (len >= 0)
                {
                    m_pushModeIODevice->write(m_buffer.constData(), len);
                }
                if (len != m_audioOutput->periodSize())
                {
                    break; // not a complete period, so buffer is completely read
                }
                --chunks;
            }
        }
        else
        {
            if (m_pushTimer)
            {
                m_pushTimer->stop();
                m_pushTimer->disconnect(this);
            }
            if (m_playMode == CNotificationSounds::SingleWithAutomaticDeletion)
            {
                this->stop();
            }
        }
    }

    void CSoundGenerator::generateData()
    {
        Q_ASSERT(m_tones.size() > 0);
        const int bytesPerSample = m_audioFormat.sampleSize() / 8;
        const int bytesForAllChannels = m_audioFormat.channelCount() * bytesPerSample;

        qint64 totalLength = 0;
        foreach(Tone t, m_tones)
        {
            totalLength += m_audioFormat.sampleRate() * bytesForAllChannels * t.m_durationMs / 1000;
        }

        Q_ASSERT(totalLength % bytesForAllChannels == 0);
        Q_UNUSED(bytesForAllChannels) // suppress warning in release builds

        m_buffer.resize(totalLength);
        unsigned char *bufferPointer = reinterpret_cast<unsigned char *>(m_buffer.data()); // clazy:exclude=detaching-member

        foreach(Tone t, m_tones)
        {
            qint64 bytesPerTone = m_audioFormat.sampleRate() * bytesForAllChannels * t.m_durationMs / 1000;
            qint64 last0AmplitudeSample = bytesPerTone; // last sample when amplitude was 0
            int sampleIndexPerTone = 0;
            while (bytesPerTone)
            {
                // http://hyperphysics.phy-astr.gsu.edu/hbase/audio/sumdif.html
                // http://math.stackexchange.com/questions/164369/how-do-you-calculate-the-frequency-perceived-by-humans-of-two-sinusoidal-waves-a
                const double pseudoTime = double(sampleIndexPerTone % m_audioFormat.sampleRate()) / m_audioFormat.sampleRate();
                double amplitude = 0.0; // amplitude -1 -> +1 , 0 is silence
                if (t.m_frequencyHz > 10)
                {
                    // the combination of two frequencies actually would have 2*amplitude,
                    // but I have to normalize with amplitude -1 -> +1

                    amplitude = t.m_secondaryFrequencyHz == 0 ?
                                qSin(2 * M_PI * t.m_frequencyHz * pseudoTime) :
                                qSin(M_PI * (t.m_frequencyHz + t.m_secondaryFrequencyHz) * pseudoTime) *
                                qCos(M_PI * (t.m_frequencyHz - t.m_secondaryFrequencyHz) * pseudoTime);
                }

                // avoid overflow
                Q_ASSERT(amplitude <= 1.0 && amplitude >= -1.0);
                if (amplitude < -1.0)
                {
                    amplitude = -1.0;
                }
                else if (amplitude > 1.0)
                {
                    amplitude = 1.0;
                }
                else if (qAbs(amplitude) < double(1.0 / 65535))
                {
                    amplitude = 0;
                    last0AmplitudeSample = bytesPerTone;
                }

                // generate this for all channels, usually 1 channel
                for (int i = 0; i < m_audioFormat.channelCount(); ++i)
                {
                    this->writeAmplitudeToBuffer(amplitude, bufferPointer);
                    bufferPointer += bytesPerSample;
                    bytesPerTone -= bytesPerSample;
                }
                ++sampleIndexPerTone;
            }

            // fixes the range from the last 0 pass through
            if (last0AmplitudeSample > 0)
            {
                bufferPointer -= last0AmplitudeSample;
                while (last0AmplitudeSample)
                {
                    double amplitude = 0.0; // amplitude -1 -> +1 , 0 is silence

                    // generate this for all channels, usually 1 channel
                    for (int i = 0; i < m_audioFormat.channelCount(); ++i)
                    {
                        this->writeAmplitudeToBuffer(amplitude, bufferPointer);
                        bufferPointer += bytesPerSample;
                        last0AmplitudeSample -= bytesPerSample;
                    }
                }
            }
        }
    }

    void CSoundGenerator::writeAmplitudeToBuffer(const double amplitude, unsigned char *bufferPointer)
    {
        if (m_audioFormat.sampleSize() == 8 && m_audioFormat.sampleType() == QAudioFormat::UnSignedInt)
        {
            const quint8 value = static_cast<quint8>((1.0 + amplitude) / 2 * 255);
            *reinterpret_cast<quint8 *>(bufferPointer) = value;
        }
        else if (m_audioFormat.sampleSize() == 8 && m_audioFormat.sampleType() == QAudioFormat::SignedInt)
        {
            const qint8 value = static_cast<qint8>(amplitude * 127);
            *reinterpret_cast<qint8 *>(bufferPointer) = value;
        }
        else if (m_audioFormat.sampleSize() == 16 && m_audioFormat.sampleType() == QAudioFormat::UnSignedInt)
        {
            quint16 value = static_cast<quint16>((1.0 + amplitude) / 2 * 65535);
            if (m_audioFormat.byteOrder() == QAudioFormat::LittleEndian)
            {
                qToLittleEndian<quint16>(value, bufferPointer);
            }
            else
            {
                qToBigEndian<quint16>(value, bufferPointer);
            }
        }
        else if (m_audioFormat.sampleSize() == 16 && m_audioFormat.sampleType() == QAudioFormat::SignedInt)
        {
            qint16 value = static_cast<qint16>(amplitude * 32767);
            if (m_audioFormat.byteOrder() == QAudioFormat::LittleEndian)
            {
                qToLittleEndian<qint16>(value, bufferPointer);
            }
            else
            {
                qToBigEndian<qint16>(value, bufferPointer);
            }
        }
    }

    bool CSoundGenerator::saveToWavFile(const QString &fileName) const
    {
        QFile file(fileName);
        bool success = file.open(QIODevice::WriteOnly);
        if (!success) return false;

        CombinedHeader header;
        constexpr auto headerLength = sizeof(CombinedHeader);
        memset(&header, 0, headerLength);

        // RIFF header
        if (m_audioFormat.byteOrder() == QAudioFormat::LittleEndian)
            memcpy(&header.riff.descriptor.id[0], "RIFF", 4);
        else
            memcpy(&header.riff.descriptor.id[0], "RIFX", 4);

        qToLittleEndian<quint32>(quint32(m_buffer.size() + headerLength - 8),
                                 reinterpret_cast<unsigned char *>(&header.riff.descriptor.size));
        memcpy(&header.riff.type[0], "WAVE", 4);

        // WAVE header
        memcpy(&header.wave.descriptor.id[0], "fmt ", 4);
        qToLittleEndian<quint32>(quint32(16),
                                 reinterpret_cast<unsigned char *>(&header.wave.descriptor.size));
        qToLittleEndian<quint16>(quint16(1),
                                 reinterpret_cast<unsigned char *>(&header.wave.audioFormat));
        qToLittleEndian<quint16>(quint16(m_audioFormat.channelCount()),
                                 reinterpret_cast<unsigned char *>(&header.wave.numChannels));
        qToLittleEndian<quint32>(quint32(m_audioFormat.sampleRate()),
                                 reinterpret_cast<unsigned char *>(&header.wave.sampleRate));
        qToLittleEndian<quint32>(quint32(m_audioFormat.sampleRate() * m_audioFormat.channelCount() * m_audioFormat.sampleSize() / 8),
                                 reinterpret_cast<unsigned char *>(&header.wave.byteRate));
        qToLittleEndian<quint16>(quint16(m_audioFormat.channelCount() * m_audioFormat.sampleSize() / 8),
                                 reinterpret_cast<unsigned char *>(&header.wave.blockAlign));
        qToLittleEndian<quint16>(quint16(m_audioFormat.sampleSize()),
                                 reinterpret_cast<unsigned char *>(&header.wave.bitsPerSample));

        // DATA header
        memcpy(&header.data.descriptor.id[0], "data", 4);
        qToLittleEndian<quint32>(quint32(m_buffer.size()),
                                 reinterpret_cast<unsigned char *>(&header.data.descriptor.size));

        success = file.write(reinterpret_cast<const char *>(&header), headerLength) == headerLength;
        success = success && file.write(m_buffer) == m_buffer.size();
        file.close();
        return success;
    }

    qint64 CSoundGenerator::calculateDurationMs(const QList<CSoundGenerator::Tone> &tones)
    {
        if (tones.isEmpty()) { return 0; }
        qint64 d = 0;
        foreach(Tone t, tones)
        {
            d += t.m_durationMs;
        }
        return d;
    }

    qint64 CSoundGenerator::readData(char *data, qint64 len)
    {
        if (len < 1) { return 0; }
        if (m_endReached)
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
            m_position = (m_position + chunkSize) % m_buffer.size();
            total += chunkSize;
            if (m_position == 0 &&
                    (m_playMode == CNotificationSounds::Single || m_playMode == CNotificationSounds::SingleWithAutomaticDeletion))
            {
                m_endReached = true;
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
        format.setSampleSize(16); // 8 or 16 works
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);
        return format;
    }

    QAudioDeviceInfo CSoundGenerator::findClosestOutputDevice(const CAudioDeviceInfo &audioDevice)
    {
        Q_ASSERT(audioDevice.getType() == CAudioDeviceInfo::OutputDevice);
        const QString lookFor = audioDevice.getName().toLower();
        QAudioDeviceInfo qtDevice = QAudioDeviceInfo::defaultOutputDevice();
        if (lookFor.startsWith("default")) { return qtDevice; }
        int score = 0;
        for (const QAudioDeviceInfo &qd : QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        {
            const QString cn = qd.deviceName().toLower();
            if (lookFor == cn) { return qd;  } // exact match
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

    CSoundGenerator *CSoundGenerator::playSignal(int volume, const QList<CSoundGenerator::Tone> &tones, const QAudioDeviceInfo &device)
    {
        CSoundGenerator *generator = new CSoundGenerator(device, CSoundGenerator::defaultAudioFormat(), tones, CNotificationSounds::SingleWithAutomaticDeletion);
        if (tones.isEmpty()) { return generator; } // that was easy
        if (volume < 1) { return generator; }
        if (generator->singleCyleDurationMs() < 10) { return generator; } // unable to hear

        // play, and maybe clean up when done
        generator->start(volume);
        return generator;
    }

    CSoundGenerator *CSoundGenerator::playSignalInBackground(int volume, const QList<CSoundGenerator::Tone> &tones, const QAudioDeviceInfo &device)
    {
        CSoundGenerator *generator = new CSoundGenerator(device, CSoundGenerator::defaultAudioFormat(), tones, CNotificationSounds::SingleWithAutomaticDeletion);
        if (tones.isEmpty()) { return generator; } // that was easy
        if (volume < 1) { return generator; }
        if (generator->singleCyleDurationMs() < 10) { return generator; } // unable to hear

        // play, and maybe clean up when done
        generator->startInOwnThread(volume);
        return generator;
    }

    void CSoundGenerator::playSignalRecorded(int volume, const QList<CSoundGenerator::Tone> &tones, const QAudioDeviceInfo &device)
    {
        if (tones.isEmpty()) { return; } // that was easy
        if (volume < 1) { return; }

        CSoundGenerator *generator = new CSoundGenerator(device, CSoundGenerator::defaultAudioFormat(), tones, CNotificationSounds::SingleWithAutomaticDeletion);
        if (generator->singleCyleDurationMs() > 10)
        {
            // play, and maybe clean up when done
            QString fileName = QString("blacksound").append(QString::number(QDateTime::currentMSecsSinceEpoch())).append(".wav");
            fileName = QDir::temp().filePath(fileName);
            generator->generateData();
            generator->saveToWavFile(fileName);
            CSoundGenerator::playFile(volume, fileName, true);
        }
        generator->deleteLater();
    }

    void CSoundGenerator::playSelcal(int volume, const CSelcal &selcal, const QAudioDeviceInfo &device)
    {
        QList<CSoundGenerator::Tone> tones;
        if (selcal.isValid())
        {
            QList<CFrequency> frequencies = selcal.getFrequencies();
            Q_ASSERT(frequencies.size() == 4);
            const CTime oneSec(1000.0, CTimeUnit::ms());
            Tone t1(frequencies.at(0), frequencies.at(1), oneSec);
            Tone t2(CFrequency(), oneSec / 5.0);
            Tone t3(frequencies.at(2), frequencies.at(3), oneSec);
            tones << t1 << t2 << t3;
        }
        CSoundGenerator::playSignalInBackground(volume, tones, device);
        // CSoundGenerator::playSignalRecorded(volume, tones, device);
    }

    void CSoundGenerator::playSelcal(int volume, const CSelcal &selcal, const CAudioDeviceInfo &audioDevice)
    {
        if (CSoundGenerator::s_selcalStarted.msecsTo(QDateTime::currentDateTimeUtc()) < 2500) return; // simple check not to play 2 SELCAL at the same time
        CSoundGenerator::s_selcalStarted = QDateTime::currentDateTimeUtc();
        CSoundGenerator::playSelcal(volume, selcal, CSoundGenerator::findClosestOutputDevice(audioDevice));
    }

    void CSoundGenerator::playNotificationSound(int volume, CNotificationSounds::Notification notification)
    {
        QMediaPlayer *mediaPlayer = CSoundGenerator::mediaPlayer();
        if (mediaPlayer->state() == QMediaPlayer::PlayingState) return;
        QMediaPlaylist *playlist = mediaPlayer->playlist();
        if (!playlist || playlist->isEmpty())
        {
            // order here is crucial, needs to be the same as in CSoundGenerator::Notification
            if (!playlist) playlist = new QMediaPlaylist(mediaPlayer);
            bool success = true;
            success = playlist->addMedia(QUrl::fromLocalFile(CDirectoryUtils::soundFilesDirectory() + "/error.wav")) && success;
            success = playlist->addMedia(QUrl::fromLocalFile(CDirectoryUtils::soundFilesDirectory() + "/login.wav")) && success;
            success = playlist->addMedia(QUrl::fromLocalFile(CDirectoryUtils::soundFilesDirectory() + "/logoff.wav")) && success;
            success = playlist->addMedia(QUrl::fromLocalFile(CDirectoryUtils::soundFilesDirectory() + "/privatemessage.wav")) && success;
            success = playlist->addMedia(QUrl::fromLocalFile(CDirectoryUtils::soundFilesDirectory() + "/voiceroomjoined.wav")) && success;
            success = playlist->addMedia(QUrl::fromLocalFile(CDirectoryUtils::soundFilesDirectory() + "/voiceroomleft.wav")) && success;

            Q_ASSERT(success);
            playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
            mediaPlayer->setPlaylist(playlist);
        }
        if (notification == CNotificationSounds::NotificationsLoadSounds) return;
        int index = static_cast<int>(notification);
        playlist->setCurrentIndex(index);
        mediaPlayer->setVolume(volume); // 0-100
        mediaPlayer->play();
    }

    void CSoundGenerator::playFile(int volume, const QString &file, bool removeFileAfterPlaying)
    {
        if (!QFile::exists(file)) { return; }
        QMediaPlayer *mediaPlayer = CSoundGenerator::mediaPlayer();
        QMediaResource mediaResource(QUrl(file), "audio");
        QMediaContent media(mediaResource);
        mediaPlayer->setMedia(media);
        mediaPlayer->setVolume(volume); // 0-100
        mediaPlayer->play();
        // I cannot delete the file here, only after it has been played
        if (removeFileAfterPlaying) { new CTimedFileDeleter(file, 1000 * 60, QCoreApplication::instance()); }
    }

    void CSoundGenerator::printAllQtSoundDevices(QTextStream &out)
    {
        out << "output:" << endl;
        for (const QAudioDeviceInfo &qd : QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        {
            out << qd.deviceName() << endl;
        }

        out << "input:" << endl;
        for (const QAudioDeviceInfo &qd : QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
        {
            out << qd.deviceName() << endl;
        }
    }
} // namespace
