// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

        //! @{
        //! Delay plus/minus
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
