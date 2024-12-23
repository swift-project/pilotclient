// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_RESOURCESOUND_H
#define SWIFT_SOUND_SAMPLEPROVIDER_RESOURCESOUND_H

#include <atomic>

#include <QExplicitlySharedDataPointer>
#include <QString>
#include <QVector>

#include "misc/worker.h"
#include "sound/swiftsoundexport.h"
#include "sound/wav/wavfile.h"

namespace swift::sound::sample_provider
{
    //! CResourceSound shared data
    struct CResourceSoundData : public QSharedData
    {
        QString fileName; //!< file name
        bool isLoaded = false; //!< is audio loaded
        QVector<float> samples; //!< audio samples
    };

    //! File from resources
    class CResourceSound
    {
    public:
        //! Constructor
        CResourceSound();

        //! Sound of audio file
        CResourceSound(const QString &audioFileName);

        //! Load the attached resource file
        bool load();

        //! Is resource already loaded?
        bool isLoaded() { return m_data->isLoaded; }

        //! Audio data
        const QVector<float> &audioData() const { return m_data->samples; }

        //! Corresponding file
        const QString &getFileName() const;

        //! Is same file?
        bool isSameFileName(const QString &fn) const;

    private:
        QExplicitlySharedDataPointer<CResourceSoundData> m_data;
    };
} // namespace swift::sound::sample_provider

#endif // SWIFT_SOUND_SAMPLEPROVIDER_RESOURCESOUND_H
