// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DATASTOREUTILITY_H
#define SWIFT_MISC_DB_DATASTOREUTILITY_H

#include <QDateTime>
#include <QString>

#include "misc/logcategories.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    class CStatusMessageList;

    namespace db
    {
        /*!
         * Class with datastore related utilities
         */
        class SWIFT_MISC_EXPORT CDatastoreUtility
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
    } // namespace db
} // namespace swift::misc

#endif // SWIFT_MISC_DB_DATASTOREUTILITY_H
