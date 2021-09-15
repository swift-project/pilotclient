/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKORE_AFV_AUDIO_CALLSIGNDELAYCACHE_H
#define BLACKORE_AFV_AUDIO_CALLSIGNDELAYCACHE_H

#include <QHash>
#include <QString>

namespace BlackCore::Afv::Audio
{
    //! Callsign delay cache
    class CallsignDelayCache
    {
    public:
        //! Initialize
        void initialise(const QString &callsign);

        //! Callsign index
        int get(const QString &callsign);

        //! Underflow
        void underflow(const QString &callsign);

        //! Success
        void success(const QString &callsign);

        //! Delay plus/minus
        //! @{
        void increaseDelayMs(const QString &callsign);
        void decreaseDelayMs(const QString &callsign);
        //! @}

        //! Singleton
        static CallsignDelayCache &instance();

    private:
        //! Ctor
        CallsignDelayCache() = default;

        static constexpr int delayDefault = 60;
        static constexpr int delayMin = 40;
        static constexpr int delayIncrement = 20;
        static constexpr int delayMax = 300;

        QHash<QString, int> m_delayCache;
        QHash<QString, int> successfulTransmissionsCache;
    };

} // ns

#endif // guard
