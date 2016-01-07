/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/stringutils.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimeZone>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
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
        QString n(ks.mid(i1 + 1, i2 - i1 - 1));
        ok = false;
        key = n.toInt(&ok);
        return ok ? key : -1;
    }

    QDateTime CDatastoreUtility::parseTimestamp(const QString &timestamp)
    {
        if (!timestamp.isEmpty())
        {
            QString ts(timestamp.trimmed().remove(' ').remove('-').remove(':')); // normalize
            QDateTime dt = QDateTime::fromString(ts, "yyyyMMddHHmmss");
            dt.setTimeZone(QTimeZone::utc());
            return dt;
        }
        else
        {
            return QDateTime();
        }
    }

    bool CDatastoreUtility::parseSwiftPublishResponse(const QString &jsonResponse, CAircraftModelList &publishedModels,  CAircraftModelList &skippedModels, CStatusMessageList &messages)
    {
        static const CLogCategoryList cats({ CLogCategory::swiftDbWebservice()});
        if (jsonResponse.isEmpty())
        {
            messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Empty JSON data"));
            return false;
        }

        QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonResponse.toUtf8()));

        // array of messages
        if (jsonDoc.isArray())
        {
            CStatusMessageList msgs(CStatusMessageList::fromDatabaseJson(jsonDoc.array()));
            messages.push_back(msgs);
            return true;
        }

        // no object -> most likely some fucked up HTML string with the PHP error
        if (!jsonDoc.isObject())
        {
            QString phpError(jsonResponse);
            phpError.remove(QRegExp("<[^>]*>"));
            messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, phpError));
            return false;
        }

        // fully blown object
        QJsonObject json(jsonDoc.object());
        bool data = false;
        if (json.contains("msgs"))
        {
            QJsonValue msgJson(json.take("msgs"));
            CStatusMessageList msgs(CStatusMessageList::fromDatabaseJson(msgJson.toArray()));
            if (!msgs.isEmpty())
            {
                messages.push_back(msgs);
                data = true;
            }
        }

        if (json.contains("publishedModels"))
        {
            QJsonValue publishedJson(json.take("publishedModels"));
            CAircraftModelList published = CAircraftModelList::fromDatabaseJson(publishedJson.toArray(), "");
            if (!published.isEmpty())
            {
                publishedModels.push_back(published);
                data = true;
            }
        }

        if (json.contains("skippedModels"))
        {
            QJsonValue skippedJson(json.take("skippedModels"));
            CAircraftModelList skipped = CAircraftModelList::fromDatabaseJson(skippedJson.toArray(), "");
            if (!skipped.isEmpty())
            {
                skippedModels.push_back(skipped);
                data = true;
            }
        }

        if (!data)
        {
            messages.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Received response, but no JSON data"));
        }

        return data;
    }

} // namespace
