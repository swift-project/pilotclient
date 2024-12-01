// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DBFLAGS_H
#define SWIFT_MISC_DB_DBFLAGS_H

#include <QFlags>
#include <QMetaType>
#include <QString>

#include "misc/swiftmiscexport.h"

namespace swift::misc::db
{
    /*!
     * What and how to read web services
     */
    class SWIFT_MISC_EXPORT CDbFlags
    {
    public:
        //! Which data to read, requires corresponding readers
        enum DataRetrievalModeFlag
        {
            Unspecified = 0, //!< Unspecified
            DbReading = 1 << 0, //!< directly from DB
            DbWriting = 1 << 1, //!< DB writing
            Shared = 1 << 2, //!< shared directory
            SharedInfoOnly = 1 << 3, //!< shared info file only
            Cached = 1 << 4, //!< from cache
            Canceled = 1 << 5, //!< canceled DB reading
            Ignore = 1 << 6, //!< ignore this entity
            CacheThenDb = DbReading | Cached, //!< Cache where possible, otherwise DB
            CacheThenShared = Shared | Cached, //!< Cache where possible, otherwise shared
            CacheAndSharedInfo = SharedInfoOnly | Cached, //!< Cached data plus shared info file
            DbReadingOrShared = DbReading | Shared, //!< read from DB or shared
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
} // namespace swift::misc::db

Q_DECLARE_METATYPE(swift::misc::db::CDbFlags::DataRetrievalModeFlag)
Q_DECLARE_METATYPE(swift::misc::db::CDbFlags::DataRetrievalMode)

#endif // SWIFT_MISC_DB_DBFLAGS_H
