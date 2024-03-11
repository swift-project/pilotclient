// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CWEBREADERFLAGS_H
#define BLACKCORE_CWEBREADERFLAGS_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/entityflags.h"

#include <QFlags>
#include <QMetaType>

namespace BlackCore
{
    /*!
     * Flags reg. the web readers
     */
    class BLACKCORE_EXPORT CWebReaderFlags
    {
    public:
        //! Which readers to init
        enum WebReaderFlag
        {
            None = 0, //!< no reader at all
            IcaoDataReader = 1 << 4, //!< reader for ICAO data
            ModelReader = 1 << 5, //!< reader for model data such as liveries, models, etc
            AirportReader = 1 << 6, //!< reader for airport list
            DbInfoDataReader = 1 << 7, //!< DB info data (metdata, how many data, when updated)
            AllReaders = IcaoDataReader | ModelReader | DbInfoDataReader | AirportReader, //!< everything
        };
        Q_DECLARE_FLAGS(WebReader, WebReaderFlag)

        //! Relationship between readers and entities
        static WebReader entitiesToReaders(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Cast
        static WebReader webReaderFlagToWebReader(WebReaderFlag flag);

        //! All entities readers can read
        static BlackMisc::Network::CEntityFlags::Entity allEntitiesForReaders(WebReader readers);

        //! Reads from swift DB?
        static bool isFromSwiftDb(BlackMisc::Network::CEntityFlags::Entity entity);

        //! Reader for swift DB?
        static bool isFromSwiftDb(WebReader reader);

        //! Number of readers
        static int numberOfReaders(WebReader readers);

        //! Is single reader
        static bool isSingleReader(WebReader readers);
    };
} // namespace

Q_DECLARE_METATYPE(BlackCore::CWebReaderFlags::WebReaderFlag)
Q_DECLARE_METATYPE(BlackCore::CWebReaderFlags::WebReader)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::CWebReaderFlags::WebReader)

#endif
