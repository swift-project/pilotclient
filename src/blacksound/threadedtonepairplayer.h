/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_THREADEDTONEPAIRPLAYER_H
#define BLACKSOUND_THREADEDTONEPAIRPLAYER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/tonepair.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/worker.h"

#include <QAudioOutput>
#include <QBuffer>
#include <QMap>
#include <QReadWriteLock>
#include <QtEndian>
#include <QtGlobal>

class QTimer;

namespace BlackSound
{
    //! Threaded tone player. Don't use it directly but use \sa CSelcalPlayer instead.
    class BLACKSOUND_EXPORT CThreadedTonePairPlayer : public BlackMisc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Constructor
        CThreadedTonePairPlayer(QObject *owner, const QString &name, const BlackMisc::Audio::CAudioDeviceInfo &device);

        //! Destructor
        virtual ~CThreadedTonePairPlayer() override = default;

        //! Play the list of tones.
        //! If the player is currently active, this call will be ignored.
        void play(int volume, const QList<BlackSound::CTonePair> &tonePairs);

        //! Reinitialize audio
        bool reinitializeAudio(const BlackMisc::Audio::CAudioDeviceInfo &device);

        //! Used audio device
        BlackMisc::Audio::CAudioDeviceInfo getAudioDevice() const;

    protected:
        //! \copydoc BlackMisc::CContinuousWorker::initialize
        virtual void initialize() override;

        //! \copydoc BlackMisc::CContinuousWorker::beforeQuit
        virtual void beforeQuit() noexcept override;

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

        BlackMisc::Audio::CAudioDeviceInfo m_deviceInfo;
        QAudioOutput *m_audioOutput = nullptr;
        QByteArray    m_bufferData;
        QBuffer       m_buffer;
        QAudioFormat  m_audioFormat;
        QMap<CTonePair, QByteArray> m_tonePairCache;
        mutable QMutex m_mutex { QMutex::Recursive };
    };
} // ns

#endif // guard
