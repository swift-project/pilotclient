/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/variantmap.h"

#include <QJsonValue>

namespace BlackMisc
{

    QJsonObject &CVariantMap::mergeToJson(QJsonObject &json) const
    {
        for (auto it = cbegin(); it != cend(); ++it)
        {
            json.insert(it.key(), it.value().toJson());
        }
        return json;
    }

    QJsonObject CVariantMap::toJson() const
    {
        QJsonObject json;
        mergeToJson(json);
        return json;
    }

    void CVariantMap::convertFromJson(const QJsonObject &json)
    {
        clear();
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            CVariant value;
            value.convertFromJson(it.value().toObject());
            implementationOf(*this).insert(cend(), it.key(), value);
        }
    }

    void CVariantMap::convertFromJson(const QJsonObject &json, const QStringList &keys)
    {
        clear();
        for (const auto &key : keys)
        {
            auto value = json.value(key);
            if (value.isUndefined()) { continue; }
            CVariant var;
            var.convertFromJson(value.toObject());
            insert(key, var);
        }
    }

    QJsonObject &CVariantMap::mergeToMemoizedJson(QJsonObject &json) const
    {
        for (auto it = cbegin(); it != cend(); ++it)
        {
            json.insert(it.key(), it.value().toMemoizedJson());
        }
        return json;
    }

    QJsonObject CVariantMap::toMemoizedJson() const
    {
        QJsonObject json;
        mergeToMemoizedJson(json);
        return json;
    }

    void CVariantMap::convertFromMemoizedJson(const QJsonObject &json)
    {
        clear();
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            CVariant value;
            value.convertFromMemoizedJson(it.value().toObject());
            implementationOf(*this).insert(cend(), it.key(), value);
        }
    }

    void CVariantMap::convertFromMemoizedJson(const QJsonObject &json, const QStringList &keys)
    {
        clear();
        for (const auto &key : keys)
        {
            auto value = json.value(key);
            if (value.isUndefined()) { continue; }
            CVariant var;
            var.convertFromMemoizedJson(value.toObject());
            insert(key, var);
        }
    }

}
