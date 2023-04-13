/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_RESOURCESOUND_H
#define BLACKSOUND_SAMPLEPROVIDER_RESOURCESOUND_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/wav/wavfile.h"
#include "blackmisc/worker.h"

#include <QString>
#include <QVector>
#include <QExplicitlySharedDataPointer>
#include <atomic>

namespace BlackSound::SampleProvider
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
} // ns

#endif // guard
