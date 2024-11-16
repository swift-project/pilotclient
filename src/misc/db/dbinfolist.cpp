// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/db/dbinfolist.h"

using namespace swift::misc::network;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::db, CDbInfo, CDbInfoList)

namespace swift::misc::db
{
    CDbInfoList::CDbInfoList() {}

    CDbInfoList::CDbInfoList(const CSequence<CDbInfo> &other) : CSequence<CDbInfo>(other) {}

    CDbInfo CDbInfoList::findFirstByEntityOrDefault(CEntityFlags::Entity entity) const
    {
        for (const CDbInfo &info : *this)
        {
            if (info.matchesEntity(entity)) { return info; }
        }
        return CDbInfo();
    }

    CDbInfoList CDbInfoList::fromDatabaseJson(const QJsonArray &array)
    {
        CDbInfoList infoObjects;
        for (const QJsonValue &value : array)
        {
            const CDbInfo info(CDbInfo::fromDatabaseJson(value.toObject()));
            infoObjects.push_back(info);
        }
        infoObjects.sortByKey(); // make sure the data are in proper order
        return infoObjects;
    }
} // namespace swift::misc::db
