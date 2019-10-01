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

#include <QString>
#include <QVector>

namespace BlackSound
{
    namespace SampleProvider
    {
        //! File from resources
        class CResourceSound
        {
        public:
            //! Sound of audio file
            CResourceSound(const QString &audioFileName);

            //! Audio data
            const QVector<float> &audioData() const { return m_samples; }

            //! Corresponding file
            const QString &getFileName() { return m_fn; }

            //! Is same file?
            bool isSameFileName(const QString &fn) const;

        private:
            QString m_fn; //!< file name
            QVector<float> m_samples;
        };
    } // ns
} // ns

#endif // guard
