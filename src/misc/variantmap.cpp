// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/variantmap.h"
#include "misc/jsonexception.h"
#include "misc/statusmessagelist.h"

#include <QJsonValue>

namespace swift::misc
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
            const QString key = it.key();
            CJsonScope scope(key);
            Q_UNUSED(scope);
            CVariant value;
            value.convertFromJson(it.value().toObject());
            implementationOf(*this).insert(cend(), key, value);
        }
    }

    void CVariantMap::convertFromJson(const QJsonObject &json, const QStringList &keys)
    {
        clear();
        for (const auto &key : keys)
        {
            auto value = json.value(key);
            if (value.isUndefined()) { continue; }
            CJsonScope scope(key);
            Q_UNUSED(scope);
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
            const QString key = it.key();
            CJsonScope scope(key);
            Q_UNUSED(scope);
            CVariant value;
            value.convertFromMemoizedJson(it.value().toObject(), true);
            implementationOf(*this).insert(cend(), key, value);
        }
    }

    void CVariantMap::convertFromMemoizedJson(const QJsonObject &json, const QStringList &keys)
    {
        clear();
        for (const auto &key : keys)
        {
            auto value = json.value(key);
            if (value.isUndefined()) { continue; }
            CJsonScope scope(key);
            Q_UNUSED(scope);
            CVariant var;
            var.convertFromMemoizedJson(value.toObject(), true);
            insert(key, var);
        }
    }

    CStatusMessageList CVariantMap::convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix)
    {
        CStatusMessageList messages;
        clear();
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            const QString key = it.key();
            CJsonScope scope(key);
            Q_UNUSED(scope);
            CVariant value;
            auto message = value.convertFromJsonNoThrow(it.value().toObject(), categories, prefix);
            if (message.isSuccess()) { implementationOf(*this).insert(cend(), key, value); }
            else { messages.push_back(message); }
        }
        return messages;
    }

    CStatusMessageList CVariantMap::convertFromJsonNoThrow(const QJsonObject &json, const QStringList &keys, const CLogCategoryList &categories, const QString &prefix)
    {
        CStatusMessageList messages;
        clear();
        for (const auto &key : keys)
        {
            auto value = json.value(key);
            if (value.isUndefined()) { continue; }
            CJsonScope scope(key);
            Q_UNUSED(scope);
            CVariant var;
            auto message = var.convertFromJsonNoThrow(value.toObject(), categories, prefix);
            if (message.isSuccess()) { insert(key, var); }
            else { messages.push_back(message); }
        }
        return messages;
    }

    CStatusMessageList CVariantMap::convertFromMemoizedJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix)
    {
        CStatusMessageList messages;
        clear();
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            const QString key = it.key();
            CJsonScope scope(key);
            Q_UNUSED(scope);
            CVariant value;
            auto message = value.convertFromMemoizedJsonNoThrow(it.value().toObject(), categories, prefix);
            if (message.isSuccess()) { implementationOf(*this).insert(cend(), key, value); }
            else { messages.push_back(message); }
        }
        return messages;
    }

    CStatusMessageList CVariantMap::convertFromMemoizedJsonNoThrow(const QJsonObject &json, const QStringList &keys, const CLogCategoryList &categories, const QString &prefix)
    {
        CStatusMessageList messages;
        clear();
        for (const auto &key : keys)
        {
            auto value = json.value(key);
            if (value.isUndefined()) { continue; }
            CJsonScope scope(key);
            Q_UNUSED(scope);
            CVariant var;
            auto message = var.convertFromMemoizedJsonNoThrow(value.toObject(), categories, prefix);
            if (message.isSuccess()) { insert(key, var); }
            else { messages.push_back(message); }
        }
        return messages;
    }
}
