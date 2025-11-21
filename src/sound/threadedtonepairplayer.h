// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_THREADEDTONEPAIRPLAYER_H
#define SWIFT_SOUND_THREADEDTONEPAIRPLAYER_H

#include <QAudioFormat>
#include <QAudioSink>
#include <QBuffer>
#include <QMap>
#include <QReadWriteLock>
#include <QtEndian>
#include <QtGlobal>

#include "misc/audio/audiodeviceinfo.h"
#include "misc/worker.h"
#include "sound/swiftsoundexport.h"
#include "sound/tonepair.h"

class QTimer;

namespace swift::sound
{
    //! Threaded tone player. Don't use it directly but use \sa CSelcalPlayer instead.
    class SWIFT_SOUND_EXPORT CThreadedTonePairPlayer : public swift::misc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Constructor
        CThreadedTonePairPlayer(QObject *owner, const QString &name,
                                const swift::misc::audio::CAudioDeviceInfo &device);

        //! Destructor
        ~CThreadedTonePairPlayer() override = default;

        //! Play the list of tones.
        //! If the player is currently active, this call will be ignored.
        void play(int volume, const QList<swift::sound::CTonePair> &tonePairs);

        //! Reinitialize audio
        bool reinitializeAudio(const swift::misc::audio::CAudioDeviceInfo &device);

        //! Used audio device
        swift::misc::audio::CAudioDeviceInfo getAudioDevice() const;

    protected:
        //! \copydoc swift::misc::CContinuousWorker::initialize
        void initialize() override;

        //! \copydoc swift::misc::CContinuousWorker::beforeQuit
        void beforeQuit() noexcept override;

    private:
        void handleStateChanged(QAudio::State newState);
        void playBuffer();
        QByteArray getAudioByTonePairs(const QList<CTonePair> &tonePairs);
        QByteArray generateAudioFromTonePairs(const CTonePair &tonePair);

        //! Write audio amplitude to data buffer
        //! This method assumes that
        //! \li sampleSize  == 16
        //! \li byte order  == little endian
        //! \li sample type == signed int
        void writeAmplitudeToBuffer(double amplitude, unsigned char *bufferPointer);

        swift::misc::audio::CAudioDeviceInfo m_deviceInfo;
        QAudioSink *m_audioOutput = nullptr;
        QByteArray m_bufferData;
        QBuffer m_buffer;
        QAudioFormat m_audioFormat;
        QMap<CTonePair, QByteArray> m_tonePairCache;
        mutable QRecursiveMutex m_mutex;
    };
} // namespace swift::sound

#endif // SWIFT_SOUND_THREADEDTONEPAIRPLAYER_H
