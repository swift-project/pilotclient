/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

    namespace Simulation { class CAircraftModelList; }
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

            //! Get data from a DB response
            static bool parseSwiftPublishResponse(const QString &jsonResponse,
                                                  Simulation::CAircraftModelList &publishedModels,
                                                  Simulation::CAircraftModelList &skippedModels,
                                                  CStatusMessageList &messages, bool &directWrite);

            //! Auto publish response
            static bool parseAutoPublishResponse(const QString &jsonResponse, CStatusMessageList &messages);
        };
    } // namespace
} // namespace

#endif // guard
