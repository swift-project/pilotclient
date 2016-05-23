/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DBFLAGS_H
#define BLACKMISC_DB_DBFLAGS_H

#include "blackmisc/blackmiscexport.h"

#include <QFlags>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Db
    {
        /*!
         * What and state of reading from web services
         */
        class BLACKMISC_EXPORT CDbFlags
        {
        public:
            //! Which data to read, requires corresponding readers
            enum DataRetrievalModeFlag
            {
                Unspecified            = 0,                  //!< Unspecified
                DbDirect               = 1 << 0,             //!< directly from DB
                Shared                 = 1 << 1,             //!< shared directory
                Cached                 = 1 << 2,             //!< from cache
                DbCached               = DbDirect | Cached,  //!< from DB, but cache when possible
                SharedCached           = Shared | Cached     //!< from shared files, but cache when possible
            };
            Q_DECLARE_FLAGS(DataRetrievalMode, DataRetrievalModeFlag)

            //! Convert to string
            static QString flagToString(DataRetrievalModeFlag flag);

            //! Convert to string
            static QString flagToString(BlackMisc::Db::CDbFlags::DataRetrievalMode flag);

            //! Register metadata
            static void registerMetadata();
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag)
Q_DECLARE_METATYPE(BlackMisc::Db::CDbFlags::DataRetrievalMode)

#endif // guard
