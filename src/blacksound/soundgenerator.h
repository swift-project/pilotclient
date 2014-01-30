/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSOUND_SOUNDGENERATOR_H
#define BLACKSOUND_SOUNDGENERATOR_H

#include "blackmisc/avselcal.h"
#include "blackmisc/vaudiodevice.h"

#include <QIODevice>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>

namespace BlackSound
{

    /*!
     * \brief Paying simple sounds
     */
    class CSoundGenerator : public QIODevice
    {
        Q_OBJECT

    public:

        /*!
         * \brief How to play
         */
        enum PlayMode
        {
            Single,
            SingleWithAutomaticDeletion,
            EndlessLoop
        };

        /*!
         * \brief Tone to be played
         */
        struct Tone
        {
            int m_frequencyHz; /*!< first tone's frequency, use 0 for silence */
            int m_secondaryFrequencyHz; /*!< second tone's frequency, or 0 */
            qint64 m_durationMs; /*!< How long to play */

            /*!
             * \brief Play frequency f for t milliseconds
             */
            Tone(int frequencyHz, qint64 durationMs) : m_frequencyHz(frequencyHz), m_secondaryFrequencyHz(0), m_durationMs(durationMs) {}

            /*!
             * \brief Play 2 frequencies f for t milliseconds
             */
            Tone(int frequencyHz, int secondaryFrequencyHz, qint64 durationMs) : m_frequencyHz(frequencyHz), m_secondaryFrequencyHz(secondaryFrequencyHz), m_durationMs(durationMs) {}
        };

        /*!
         * \brief Constructor
         * \param device        device
         * \param format        audio format
         * \param tones         list of Tones
         * \param mode          play once?
         * \param parent
         * \see PlayMode
         */
        CSoundGenerator(const QAudioDeviceInfo &device, const QAudioFormat &format, const QList<Tone> &tones, PlayMode mode, QObject *parent = nullptr);

        /*!
         * \brief Constructor
         * \param tones         list of Tones
         * \param mode          play once?
         * \param parent
         * \see PlayMode
         */
        CSoundGenerator(const QList<Tone> &tones, PlayMode mode, QObject *parent = nullptr);

        /*!
         * Destructor
         */
        ~CSoundGenerator();

        /*!
         * \brief Close device, buffer stays intact
         */
        void stop(bool destructor = false);

        /*!
         * \brief sDuration of one cycle
         */
        qint64 singleCyleDurationMs() const { return calculateDurationMs(this->m_tones); }

        /*!
         * \copydoc QIODevice::readData()
         */
        virtual qint64 readData(char *data, qint64 maxlen);

        /*!
         * \copydoc QIODevice::writeData()
         * \remarks NOT(!) used here
         */
        virtual qint64 writeData(const char *data, qint64 len);

        /*!
         * \copydoc QIODevice::bytesAvailable()
         */
        virtual qint64 bytesAvailable() const;

        /*!
         * \copydoc QIODevice::seek()
         */
        virtual bool seek(qint64 pos)
        {
            return this->m_endReached ? false : QIODevice::seek(pos);
        }

        /*!
         * \copydoc QIODevice::atEnd()
         */
        virtual bool atEnd() const
        {
            return this->m_endReached ? true : QIODevice::atEnd();
        }

        /*!
         * \brief Default audio format fo play these sounds
         * \return
         */
        static QAudioFormat defaultAudioFormat();

        /*!
         * \brief Find the closest Qt device to this audio device
         * \param audioDevice   output audio device
         * \return
         */
        static QAudioDeviceInfo findClosestOutputDevice(const BlackMisc::Voice::CAudioDevice &audioDevice);

        /*!
         * \brief Play signal of tones once
         * \param volume    0-100
         * \param tones     list of tones
         * \param device    device to be used
         */
        static void playSignal(qint32 volume, const QList<Tone> &tones, QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice());

        /*!
         * \brief Play SELCAL tone
         * \param volume    0-100
         * \param selcal
         * \param device    device to be used
         * \see BlackMisc::Aviation::CSelcal
         */
        static void playSelcal(qint32 volume, const BlackMisc::Aviation::CSelcal &selcal, QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice());

        /*!
         * \brief Play SELCAL tone
         * \param volume    0-100
         * \param selcal
         * \param audioDevice device to be used
         * \see BlackMisc::Aviation::CSelcal
         */
        static void playSelcal(qint32 volume, const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::Voice::CAudioDevice &audioDevice);

        /*!
         * \brief One cycle of tones takes t milliseconds
         */
        qint64 oneCycleDurationMs() const
        {
            return this->m_oneCycleDurationMs;
        }

    signals:
        /*!
         * \brief Device was closed
         * \remarks With singleShot the signal indicates that sound sequence has finished
         */
        void stopped();

    public slots:
        /*!
         * \brief Play sound, open device
         * \param volume 0..100
         */
        void start(int volume);

    private:
        /*!
         * \brief Generate tone data in internal buffer
         */
        void generateData();

    private:
        QList<Tone> m_tones; /*! tones to be played */
        qint64 m_position; /*!< position in buffer */
        bool m_playMode; /*!< end data provisioning after playing all tones, play endless loop */
        bool m_endReached; /*!< indicates end in combination with single play */
        qint64 m_oneCycleDurationMs; /*!< how long is one cycle of tones */
        QByteArray m_buffer; /*!< generated buffer for data */
        QAudioDeviceInfo m_device; /*!< audio device */
        QAudioFormat m_audioFormat; /*!< used format */
        QScopedPointer<QAudioOutput> m_audioOutput;

        /*!
         * \brief Duration of these tones
         */
        static qint64 calculateDurationMs(const QList<Tone> &tones);

    };
} //namespace


#endif // guard
