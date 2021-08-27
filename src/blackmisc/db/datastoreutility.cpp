/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/stringutils.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimeZone>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Db
    {
        const QStringList &CDatastoreUtility::getLogCategories()
        {
            static const QStringList cats({CLogCategories::swiftDbWebservice()});
            return cats;
        }

        bool CDatastoreUtility::dbBoolStringToBool(const QString &dbBool)
        {
            return BlackMisc::stringToBool(dbBool);
        }

        const QString &CDatastoreUtility::boolToDbYN(bool v)
        {
            static const QString y("Y");
            static const QString n("N");
            return v ? y : n;
        }

        int CDatastoreUtility::extractIntegerKey(const QString &stringWithKey)
        {
            QString ks(stringWithKey.trimmed());
            if (ks.isEmpty()) { return -1; }
            bool ok = false;
            int key = ks.toInt(&ok);
            if (ok) { return key; } // only a number

            // key in string with ()
            int i1 = ks.lastIndexOf('(');
            if (i1 < 0) { return -1; }
            int i2 = ks.lastIndexOf(')');
            if (i2 <= i1 + 1) { return -1;}
            const QString n(ks.mid(i1 + 1, i2 - i1 - 1));
            ok = false;
            key = n.toInt(&ok);
            return ok ? key : -1;
        }

        QString CDatastoreUtility::stripKeyInParentheses(const QString &valueWithKey)
        {
            const int i = valueWithKey.indexOf('(');
            if (i < 0) { return valueWithKey.trimmed(); }
            if (i < 1) { return {}; }
            return valueWithKey.left(i - 1).trimmed();
        }

        QDateTime CDatastoreUtility::parseTimestamp(const QString &timestamp)
        {
            if (timestamp.isEmpty()) { return QDateTime(); }
            return parseDateTimeStringOptimized(removeDateTimeSeparators(timestamp));
        }

        bool CDatastoreUtility::parseAutoPublishResponse(const QString &jsonResponse, CStatusMessageList &messages)
        {
            if (jsonResponse.isEmpty())
            {
                messages.push_back(CStatusMessage(static_cast<CDatastoreUtility *>(nullptr), CStatusMessage::SeverityError, u"Empty JSON data for published models"));
                return false;
            }

            const QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonResponse.toUtf8()));

            // array of messages only
            if (jsonDoc.isArray())
            {
                const CStatusMessageList msgs(CStatusMessageList::fromDatabaseJson(jsonDoc.array()));
                messages.push_back(msgs);
                return true;
            }

            // no object -> most likely some fucked up HTML string with the PHP error
            if (!jsonDoc.isObject())
            {
                const QString phpError(CNetworkUtils::removeHtmlPartsFromPhpErrorMessage(jsonResponse));
                messages.push_back(CStatusMessage(static_cast<CDatastoreUtility *>(nullptr), CStatusMessage::SeverityError, phpError));
                return false;
            }

            QJsonObject json(jsonDoc.object());
            if (json.contains("msgs"))
            {
                const QJsonValue msgJson(json.take("msgs"));
                const CStatusMessageList msgs(CStatusMessageList::fromDatabaseJson(msgJson.toArray()));
                if (!msgs.isEmpty())
                {
                    messages.push_back(msgs);
                    return !messages.hasErrorMessages();
                }
            }
            return false;
        }
    } // ns
} // ns
