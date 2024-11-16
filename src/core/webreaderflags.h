// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CWEBREADERFLAGS_H
#define SWIFT_CORE_CWEBREADERFLAGS_H

#include <QFlags>
#include <QMetaType>

#include "core/swiftcoreexport.h"
#include "misc/network/entityflags.h"

namespace swift::core
{
    /*!
     * Flags reg. the web readers
     */
    class SWIFT_CORE_EXPORT CWebReaderFlags
    {
    public:
        //! Which readers to init
        enum WebReaderFlag
        {
            None = 0, //!< no reader at all
            VatsimDataReader = 1 << 0, //!< reader for VATSIM data
            VatsimMetarReader = 1 << 1, //!< reader for VATSIM metar data
            VatsimStatusReader = 1 << 2, //!< reader for VATSIM status file
            VatsimServerFileReader = 1 << 3, //!< reader for VATSIM server file
            IcaoDataReader = 1 << 4, //!< reader for ICAO data
            ModelReader = 1 << 5, //!< reader for model data such as liveries, models, etc
            AirportReader = 1 << 6, //!< reader for airport list
            DbInfoDataReader = 1 << 7, //!< DB info data (metdata, how many data, when updated)
            AllVatsimReaders = VatsimDataReader | VatsimMetarReader | VatsimStatusReader | VatsimServerFileReader, //!< all VATSIM readers
            AllSwiftDbReaders = IcaoDataReader | ModelReader | DbInfoDataReader | AirportReader, //!< all swift data
            AllReaders = AllSwiftDbReaders | AllVatsimReaders //!< everything
        };
        Q_DECLARE_FLAGS(WebReader, WebReaderFlag)

        //! Relationship between readers and entities
        static WebReader entitiesToReaders(swift::misc::network::CEntityFlags::Entity entities);

        //! Cast
        static WebReader webReaderFlagToWebReader(WebReaderFlag flag);

        //! All entities readers can read
        static swift::misc::network::CEntityFlags::Entity allEntitiesForReaders(WebReader readers);

        //! Reads from swift DB?
        static bool isFromSwiftDb(swift::misc::network::CEntityFlags::Entity entity);

        //! Reader for swift DB?
        static bool isFromSwiftDb(WebReader reader);

        //! Number of readers
        static int numberOfReaders(WebReader readers);

        //! Is single reader
        static bool isSingleReader(WebReader readers);
    };
} // namespace swift::core

Q_DECLARE_METATYPE(swift::core::CWebReaderFlags::WebReaderFlag)
Q_DECLARE_METATYPE(swift::core::CWebReaderFlags::WebReader)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::core::CWebReaderFlags::WebReader)

#endif
