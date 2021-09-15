/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef WAVFILE_H
#define WAVFILE_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>

namespace BlackSound::Wav
{
    //! * WAV file
    class CWavFile : public QFile
    {
    public:
        //! Ctor
        CWavFile(QObject *parent = nullptr);

        //! Standard open
        using QFile::open;

        //! Open
        bool open(const QString &fileName);

        //! Audio format
        const QAudioFormat &fileFormat() const;

        //! Header length
        qint64 headerLength() const;

        //! The audio data
        const QByteArray &audioData() const { return m_audioData; }

    private:
        bool readHeader();

        QAudioFormat m_fileFormat;
        qint64 m_headerLength;
        QByteArray m_audioData;
    };
} // ns

#endif // guard
