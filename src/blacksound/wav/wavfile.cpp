/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include <qendian.h>
#include <QVector>
#include <QDebug>
#include "wavfile.h"
// #include "utils.h"

namespace BlackSound::Wav
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

    CWavFile::CWavFile(QObject *parent) : QFile(parent),
                                          m_headerLength(0)
    {}

    bool CWavFile::open(const QString &fileName)
    {
        this->close();
        this->setFileName(fileName);
        return QFile::open(QIODevice::ReadOnly) && readHeader();
    }

    const QAudioFormat &CWavFile::fileFormat() const
    {
        return m_fileFormat;
    }

    qint64 CWavFile::headerLength() const
    {
        return m_headerLength;
    }

    bool CWavFile::readHeader()
    {
        seek(0);
        CombinedHeader header;
        DATAHeader dataHeader;
        bool result = read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
        if (result)
        {
            if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0 || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0) && memcmp(&header.riff.type, "WAVE", 4) == 0 && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0 && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0 || header.wave.audioFormat == 3))
            {
                // Read off remaining header information
                if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader))
                {
                    // Extended data available
                    quint16 extraFormatBytes;
                    if (peek((char *)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                        return false;
                    const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                    if (read(throwAwayBytes).size() != throwAwayBytes)
                        return false;
                }

                if (read((char *)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                    return false;

                // Establish format
                if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                    m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
                else
                    m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

                int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
                m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
                m_fileFormat.setCodec("audio/pcm");
                m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
                m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));

                if (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)
                {
                    m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
                }
                else
                {
                    m_fileFormat.setSampleType(QAudioFormat::Float);
                }
            }
            else
            {
                result = false;
            }
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
} // ns
