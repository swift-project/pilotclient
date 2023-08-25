// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_DB_DATASTOREUTILITY_H
#define BLACKMISC_DB_DATASTOREUTILITY_H

#include "blackmisc/logcategories.h"
#include "blackmisc/blackmiscexport.h"

#include <QDateTime>
#include <QString>

namespace BlackMisc
{
    class CStatusMessageList;

    namespace Db
    {
        /*!
         * Class with datastore related utilities
         */
        class BLACKMISC_EXPORT CDatastoreUtility
        {
        public:
            //! Log.categories
            static const QStringList &getLogCategories();

            //! No constructor
            CDatastoreUtility() = delete;

            //! DB Bool value to bool
            static bool dbBoolStringToBool(const QString &dbBool);

            //! Bool to DB yes/no
            static const QString &boolToDbYN(bool v);

            //! Extract key from string like "MyAircraft (33)"
            static int extractIntegerKey(const QString &stringWithKey);

            //! Strip the parentheses part, e.g. "foo (123)" => "foo"
            static QString stripKeyInParentheses(const QString &valueWithKey);

            //! Parse a timestamp object
            static QDateTime parseTimestamp(const QString &timestamp);

            //! Auto publish response
            static bool parseAutoPublishResponse(const QString &jsonResponse, CStatusMessageList &messages);
        };
    } // namespace
} // namespace

#endif // guard
