/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/datastoreutility.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QJsonObject>
#include <QJsonDocument>

namespace BlackMisc
{
    bool CDatastoreUtility::dbBoolStringToBool(const QString &dbBool)
    {
        return BlackMisc::stringToBool(dbBool);
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
        Q_ASSERT_X(!timestamp.isEmpty(), Q_FUNC_INFO, "Missing timestamp");
        if (!timestamp.isEmpty())
        {
            QString ts(timestamp.trimmed().remove(' ').remove('-').remove(':')); // normalize
            QDateTime dt = QDateTime::fromString(ts, "yyyyMMddHHmmss");
            return dt;
        }
        else
        {
            return QDateTime();
        }
    }

    bool CDatastoreUtility::parseSwiftWriteResponse(const QString &jsonResponse, CStatusMessageList &messages, CVariant &key)
    {
        if (jsonResponse.isEmpty()) { return ""; }
        QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonResponse.toUtf8()));
        if (!jsonDoc.isObject()) { return ""; }
        QJsonObject json(jsonDoc.object());
        Q_ASSERT_X(!json.value("id").isNull(), Q_FUNC_INFO, "malformed response");
        if (json.value("id").isNull()) { return false; }
        QString id(json.value("id").toString().trimmed());
        QJsonArray msgObject(json.value("messages").toArray());
        messages.push_back(CStatusMessageList::fromDatabaseJson(msgObject));
        bool success = false;

        int intKey;
        bool isInt;
        intKey = id.toInt(&isInt);
        if (isInt)
        {
            key.setValue(intKey);
            success = (intKey >= 0);
        }
        else
        {
            key.setValue(id);
            success = !id.isEmpty();
        }
        return success;
    }

} // namespace
