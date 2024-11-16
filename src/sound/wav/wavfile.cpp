// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "wavfile.h"

#include <qendian.h>

#include <limits> // Workaround for QTBUG-90395

#include <QDebug>
#include <QVector>
// #include "utils.h"

namespace swift::sound::wav
{
    //! WAV chunk
    struct chunk
    {
        char id[4]; //!< chunk id
        quint32 size; //!< chunk size
    };

    //! RIFF header
    struct RIFFHeader
    {
        chunk descriptor; //!< "RIFF"
        char type[4]; //!< "WAVE"
    };

    //! WAVE header
    struct WAVEHeader
    {
        chunk descriptor; //!< chunk descriptor
        quint16 audioFormat; //!< audio format, e.g. 0x0001 => PCM
        quint16 numChannels; //!< number of channels
        quint32 sampleRate; //!< sample rate
        quint32 byteRate; //!< byte rate
        quint16 blockAlign; //!< block align
        quint16 bitsPerSample; //!< bits per sample
    };

    //! Data header
    struct DATAHeader
    {
        chunk descriptor; //!< chunk descriptor
    };

    //! Combined header
    struct CombinedHeader
    {
        RIFFHeader riff; //!< RIFF header
        WAVEHeader wave; //!< WAVE header
    };

    CWavFile::CWavFile(QObject *parent) : QFile(parent), m_headerLength(0) {}

    bool CWavFile::open(const QString &fileName)
    {
        this->close();
        this->setFileName(fileName);
        return QFile::open(QIODevice::ReadOnly) && readHeader();
    }

    const QAudioFormat &CWavFile::fileFormat() const { return m_fileFormat; }

    qint64 CWavFile::headerLength() const { return m_headerLength; }

    bool CWavFile::readHeader()
    {
        seek(0);
        CombinedHeader header;
        DATAHeader dataHeader;
        bool result = read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
        if (result)
        {
            if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0 ||
                 memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0) &&
                memcmp(&header.riff.type, "WAVE", 4) == 0 && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0 &&
                (header.wave.audioFormat == 1 || header.wave.audioFormat == 0 || header.wave.audioFormat == 3))
            {
                // Read off remaining header information
                if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader))
                {
                    // Extended data available
                    quint16 extraFormatBytes;
                    if (peek((char *)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16)) return false;
                    const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                    if (read(throwAwayBytes).size() != throwAwayBytes) return false;
                }

                if (read((char *)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader)) return false;

                // Establish format
                int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
                m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
                m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));

                if (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)
                {
                    if (bps == 8) { m_fileFormat.setSampleFormat(QAudioFormat::UInt8); }
                    else if (bps == 16) { m_fileFormat.setSampleFormat(QAudioFormat::Int16); }
                    else { m_fileFormat.setSampleFormat(QAudioFormat::Int32); }
                }
                else { m_fileFormat.setSampleFormat(QAudioFormat::Float); }
            }
            else { result = false; }
        }
        m_headerLength = pos();

        if (memcmp(&dataHeader.descriptor.id, "data", 4) == 0)
        {
            const qint32 dataLength = qFromLittleEndian<qint32>(dataHeader.descriptor.size);
            m_audioData = read(dataLength);
            if (m_audioData.size() != dataLength)
            {
                m_audioData.clear();
                return false;
            }
        }
        return result;
    }
} // namespace swift::sound::wav
