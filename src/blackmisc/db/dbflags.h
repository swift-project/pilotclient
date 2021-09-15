/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DBFLAGS_H
#define BLACKMISC_DB_DBFLAGS_H

#include "blackmisc/blackmiscexport.h"

#include <QFlags>
#include <QMetaType>
#include <QString>

namespace BlackMisc::Db
{
    /*!
     * What and how to read web services
     */
    class BLACKMISC_EXPORT CDbFlags
    {
    public:
        //! Which data to read, requires corresponding readers
        enum DataRetrievalModeFlag
        {
            Unspecified           = 0,       //!< Unspecified
            DbReading             = 1 << 0,  //!< directly from DB
            DbWriting             = 1 << 1,  //!< DB writing
            Shared                = 1 << 2,  //!< shared directory
            SharedInfoOnly        = 1 << 3,  //!< shared info file only
            Cached                = 1 << 4,  //!< from cache
            Canceled              = 1 << 5,  //!< canceled DB reading
            Ignore                = 1 << 6,  //!< ignore this entity
            CacheThenDb           = DbReading | Cached,      //!< Cache where possible, otherwise DB
            CacheThenShared       = Shared | Cached,         //!< Cache where possible, otherwise shared
            CacheAndSharedInfo    = SharedInfoOnly | Cached, //!< Cached data plus shared info file
            DbReadingOrShared     = DbReading | Shared,      //!< read from DB or shared
            DbReadingOrAnyShared  = DbReading | Shared | SharedInfoOnly //!< read from DB or shared/shared info
        };
        Q_DECLARE_FLAGS(DataRetrievalMode, DataRetrievalModeFlag)

        //! Reads from web (or just cached)
        static bool readsFromWeb(CDbFlags::DataRetrievalMode mode);

        //! Convert to string
        static QString flagToString(DataRetrievalModeFlag flag);

        //! Convert to string
        static QString flagToString(CDbFlags::DataRetrievalMode mode);

        //! Mode to flag
        //! \remark any combination results in Unspecified, only single flags are returned
        static DataRetrievalModeFlag modeToModeFlag(DataRetrievalMode mode);

        //! Adjust flag as we already know DB is down
        static DataRetrievalMode adjustWhenDbIsDown(DataRetrievalMode mode);

        //! Register metadata
        static void registerMetadata();
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag)
Q_DECLARE_METATYPE(BlackMisc::Db::CDbFlags::DataRetrievalMode)

#endif // guard
