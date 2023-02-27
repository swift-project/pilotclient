/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
            None                   = 0,        //!< no reader at all
            VatsimBookingReader    = 1 << 0,   //!< reader for VATSIM booking data
            VatsimDataReader       = 1 << 1,   //!< reader for VATSIM data
            VatsimMetarReader      = 1 << 2,   //!< reader for VATSIM metar data
            VatsimStatusReader     = 1 << 3,   //!< reader for VATSIM status file
            VatsimServerFileReader = 1 << 4,   //!< reader for VATSIM server file
            IcaoDataReader         = 1 << 5,   //!< reader for ICAO data
            ModelReader            = 1 << 6,   //!< reader for model data such as liveries, models, etc
            AirportReader          = 1 << 7,   //!< reader for airport list
            DbInfoDataReader       = 1 << 8,   //!< DB info data (metdata, how many data, when updated)
            AllVatsimReaders       = VatsimBookingReader | VatsimDataReader | VatsimMetarReader | VatsimStatusReader | VatsimServerFileReader,  //!< all VATSIM readers
            AllSwiftDbReaders      = IcaoDataReader      | ModelReader      | DbInfoDataReader  | AirportReader,       //!< all swift data
            AllReaders             = AllSwiftDbReaders   | AllVatsimReaders                                            //!< everything
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
