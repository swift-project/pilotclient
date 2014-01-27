/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSOUND_SOUNDGENERATOR_H
#define BLACKSOUND_SOUNDGENERATOR_H

#include <QIODevice>
#include <QAudioFormat>
#include <QAudioDeviceInfo>

namespace BlackSound
{

    class CSoundGenerator : public QIODevice
    {
        Q_OBJECT

    public:
        /*!
         * \brief Tone to be played
         */
        struct Tone
        {
            int m_frequencyHz;
            int m_secondaryFrequencyHz;
            qint64 m_durationMs;

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
         * \param format
         * \param tones         list of Tones
         * \param singlePlay    play once?
         * \param parent
         */
        CSoundGenerator(const QAudioFormat &format, const QList<Tone> &tones, bool singlePlay, QObject *parent);


        /*!
         * \brief Constructor
         * \param tones         list of Tones
         * \param singlePlay    play once?
         * \param parent
         */
        CSoundGenerator(const QList<Tone> &tones, bool singlePlay, QObject *parent);

        /*!
         * Destructor
         */
        ~CSoundGenerator();

        /*!
         * \brief Open device
         */
        void start();

        /*!
         * \brief Close device, buffer stays intact
         */
        void stop();

        /*!
         * \copydoc QIODevice::readData()
         */
        qint64 readData(char *data, qint64 maxlen);

        /*!
         * \copydoc QIODevice::writeData()
         * \remarks NOT(!) used here
         */
        qint64 writeData(const char *data, qint64 len);

        /*!
         * \copydoc QIODevice::bytesAvailable()
         */
        qint64 bytesAvailable() const;

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
         * \brief One cycle of tones takes t milliseconds
         */
        qint64 oneCycleDurationMs() const
        {
            return this->m_oneCycleDurationMs;
        }

        /*!
         * \brief Default audio format fo play these sounds
         * \return
         */
        static QAudioFormat defaultAudioFormat();

        /*!
         * \brief Play signal of tones once
         * \param volume    0-100
         * \param tones     list of tones
         * \param device    device to be used
         */
        static void playSignal(qint32 volume, const QList<Tone> &tones, QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice());

    signals:
        /*!
         * \brief Device was closed
         * \remarks With singleShot the signal indicates that sound sequence has finished
         */
        void stopped();

    private:
        /*!
         * \brief Generate tone data in internal buffer
         */
        void generateData(const QAudioFormat &format, const QList<Tone> &tones);

    private:
        qint64 m_position; /*!< position in buffer */
        bool m_singlePlay; /*!< end data provisioning after playing all tones */
        bool m_endReached; /*!< indicates end in combination with single play */
        qint64 m_oneCycleDurationMs; /*!< how long is one cycle of tones */
        QByteArray m_buffer;

        /*!
         * \brief Duration of these tones
         */
        static qint64 calculateDurationMs(const QList<Tone> &tones);

    };
} //namespace
#endif // guard
