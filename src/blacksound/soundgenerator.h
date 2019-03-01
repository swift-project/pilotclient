/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_SOUNDGENERATOR_H
#define BLACKSOUND_SOUNDGENERATOR_H

#include "blacksoundexport.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/audio/notificationsounds.h"
#include <QIODevice>
#include <QThread>
#include <QDateTime>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QMediaPlayer>

namespace BlackSound
{
    //! Playing simple sounds
    class BLACKSOUND_EXPORT CSoundGenerator : public QIODevice
    {
        Q_OBJECT

    public:
        //! Tone to be played
        struct Tone
        {
            friend class CSoundGenerator;

        public:
            //! Play frequency f for t milliseconds
            Tone(const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::PhysicalQuantities::CTime &duration) :
                m_frequencyHz(static_cast<int>(frequency.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::Hz()))),
                m_secondaryFrequencyHz(0),
                m_durationMs(static_cast<qint64>(duration.valueRounded(BlackMisc::PhysicalQuantities::CTimeUnit::ms()))) {}

            //! Play 2 frequencies f for t milliseconds
            Tone(const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::PhysicalQuantities::CFrequency &secondaryFrequency, const BlackMisc::PhysicalQuantities::CTime &duration) :
                m_frequencyHz(static_cast<int>(frequency.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::Hz()))),
                m_secondaryFrequencyHz(static_cast<int>(secondaryFrequency.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::Hz()))),
                m_durationMs(static_cast<qint64>(duration.valueRounded(BlackMisc::PhysicalQuantities::CTimeUnit::ms()))) {}

        private:
            int m_frequencyHz;          //!< first tone's frequency, use 0 for silence
            int m_secondaryFrequencyHz; //!< second tone's frequency, or 0
            qint64 m_durationMs;        //!< How long to play (duration)
        };

        //! Constructor
        //! \param device        device
        //! \param format        audio format
        //! \param tones         list of Tones
        //! \param mode          play once?
        //! \param parent
        //! \see PlayMode
        CSoundGenerator(const QAudioDeviceInfo &device, const QAudioFormat &format, const QList<Tone> &tones, BlackMisc::Audio::CNotificationSounds::PlayMode mode, QObject *parent = nullptr);

        //! Constructor
        //! \param tones         list of Tones
        //! \param mode          play once?
        //! \param parent
        //! \see PlayMode
        CSoundGenerator(const QList<Tone> &tones, BlackMisc::Audio::CNotificationSounds::PlayMode mode, QObject *parent = nullptr);

        //! Destructor
        virtual ~CSoundGenerator() override;

        //! Set volume
        //! \param volume 0..100
        void setVolume(int volume)
        {
            m_audioOutput->setVolume(qreal(volume / 100.0));
        }

        //! Close device, buffer stays intact
        void stop(bool destructor = false);

        //! Duration of one cycle
        qint64 singleCyleDurationMs() const { return calculateDurationMs(m_tones); }

        //! \copydoc QIODevice::readData()
        virtual qint64 readData(char *data, qint64 maxlen) override;

        //! \copydoc QIODevice::writeData()
        //! \remarks NOT(!) used here
        virtual qint64 writeData(const char *data, qint64 len) override;

        //! \copydoc QIODevice::bytesAvailable()
        virtual qint64 bytesAvailable() const override;

        //! \copydoc QIODevice::seek()
        virtual bool seek(qint64 pos) override
        {
            return m_endReached ? false : QIODevice::seek(pos);
        }

        //! \copydoc QIODevice::atEnd()
        virtual bool atEnd() const override
        {
            return m_endReached ? true : QIODevice::atEnd();
        }

        //! Default audio format fo play these sounds
        static QAudioFormat defaultAudioFormat();

        //! Find the closest Qt device to this audio device
        //! \param audioDevice   output audio device
        //! \return
        static QAudioDeviceInfo findClosestOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice);

        //! Play signal of tones once
        //! \param volume    0-100
        //! \param tones     list of tones
        //! \param device    device to be used
        //! \return generator used, important with SingleWithAutomaticDeletion automatically deleted
        static CSoundGenerator *playSignal(int volume, const QList<Tone> &tones, const QAudioDeviceInfo &device = QAudioDeviceInfo::defaultOutputDevice());

        //! Play signal of tones once
        //! \param volume    0-100
        //! \param tones     list of tones
        //! \param device    device to be used
        static void playSignalInBackground(int volume, const QList<CSoundGenerator::Tone> &tones, const QAudioDeviceInfo &device);

        //! Play SELCAL tone
        //! \param volume    0-100
        //! \param selcal
        //! \param device    device to be used
        //! \see BlackMisc::Aviation::CSelcal
        static void playSelcal(int volume, const BlackMisc::Aviation::CSelcal &selcal, const QAudioDeviceInfo &device = QAudioDeviceInfo::defaultOutputDevice());

        //! Play SELCAL tone
        //! \param volume    0-100
        //! \param selcal
        //! \param audioDevice device to be used
        //! \see BlackMisc::Aviation::CSelcal
        static void playSelcal(int volume, const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::Audio::CAudioDeviceInfo &audioDevice);

        //! One cycle of tones takes t milliseconds
        BlackMisc::PhysicalQuantities::CTime oneCycleDurationMs() const
        {
            return BlackMisc::PhysicalQuantities::CTime(m_oneCycleDurationMs, BlackMisc::PhysicalQuantities::CTimeUnit::ms());
        }

        //! Play given file
        //! \param volume    0-100
        //! \param file
        //! \param removeFileAfterPlaying delete the file, after it has been played
        static void playFile(int volume, const QString &file, bool removeFileAfterPlaying);

        //! Play notification
        //! \param volume    0-100
        //! \param notification
        static void playNotificationSound(int volume, BlackMisc::Audio::CNotificationSounds::NotificationFlag notification);

        //! For debugging purposes
        static void printAllQtSoundDevices(QTextStream &qtout);

    signals:
        //! Device was closed
        //! \remarks With singleShot the signal indicates that sound sequence has finished
        void stopped();

        //! Generator is stopping
        void stopping();

    public slots:
        //! Play sound, open device
        //! \param volume 0..100
        //! \param pull pull/push, if false push mode
        void start(int volume, bool pull = true);

    private slots:
        //! Push mode, timer expired
        void pushTimerExpired();

    private:
        //! Generate tone data in internal buffer
        void generateData();

    private:
        QList<Tone> m_tones;                          //!< tones to be played
        qint64 m_position;                            //!< position in buffer
        BlackMisc::Audio::CNotificationSounds::PlayMode m_playMode; //!< end data provisioning after playing all tones, play endless loop
        bool m_endReached;                            //!< indicates end in combination with single play
        qint64 m_oneCycleDurationMs;                  //!< how long is one cycle of tones
        QByteArray m_buffer;                          //!< generated buffer for data
        QAudioDeviceInfo m_device;                    //!< audio device
        QAudioFormat m_audioFormat;                   //!< used format
        QScopedPointer<QAudioOutput> m_audioOutput;
        QTimer    *m_pushTimer        = nullptr;      //!< Push mode timer
        QIODevice *m_pushModeIODevice = nullptr;      //!< IO device when used in push mode
        QThread   *m_ownThread        = nullptr;
        static QDateTime s_selcalStarted;

        //! Header for saving .wav files
        struct chunk
        {
            char        id[4];
            quint32     size;
        };

        //! Header for saving .wav files
        struct RiffHeader
        {
            chunk       descriptor;     //!< "RIFF"
            char        type[4];        //!< "WAVE"
        };

        //! Header for saving .wav files
        struct WaveHeader
        {
            chunk       descriptor;
            quint16     audioFormat;
            quint16     numChannels;
            quint32     sampleRate;
            quint32     byteRate;
            quint16     blockAlign;
            quint16     bitsPerSample;
        };

        //! Header for saving .wav files
        struct DataHeader
        {
            chunk       descriptor;
        };

        //! Header for saving .wav files
        struct CombinedHeader
        {
            RiffHeader  riff;
            WaveHeader  wave;
            DataHeader  data;
        };

        //! "My" media player
        static QMediaPlayer *mediaPlayer()
        {
            static QMediaPlayer *mediaPlayer = new QMediaPlayer();
            return mediaPlayer;
        }

        //! Duration of these tones
        static qint64 calculateDurationMs(const QList<Tone> &tones);

        //! save buffer to wav file
        bool saveToWavFile(const QString &fileName) const;

        //! Write amplitude to buffer
        //! \param amplitude value -1 .. 1
        //! \param bufferPointer current buffer pointer
        void writeAmplitudeToBuffer(const double amplitude, unsigned char *bufferPointer);
    };
} //namespace

#endif // guard
