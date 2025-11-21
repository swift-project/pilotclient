// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef WAVFILE_H
#define WAVFILE_H

#include <QAudioFormat>
#include <QFile>
#include <QObject>

namespace swift::sound::wav
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
        qint64 m_headerLength { 0 };
        QByteArray m_audioData;
    };
} // namespace swift::sound::wav

#endif // WAVFILE_H
