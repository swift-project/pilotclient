/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef CALLSIGNDELAYCACHE_H
#define CALLSIGNDELAYCACHE_H

#include <QHash>
#include <QString>

namespace BlackCore
{
    namespace Afv
    {
        namespace Audio
        {
            //! Callsign delay cache
            class CallsignDelayCache
            {
            public:
                void initialise(const QString &callsign);
                int get(const QString &callsign);
                void underflow(const QString &callsign);
                void success(const QString &callsign);
                void increaseDelayMs(const QString &callsign);
                void decreaseDelayMs(const QString &callsign);

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
    } // ns
} // ns

#endif // guard
