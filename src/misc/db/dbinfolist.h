// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DB_DBINFOLIST_H
#define SWIFT_MISC_DB_DBINFOLIST_H

#include "misc/db/dbinfo.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/network/entityflags.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

#include <initializer_list>

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc::db, CDbInfo, CDbInfoList)

namespace swift::misc::db
{
    //! Value object encapsulating a list of info objects.
    class SWIFT_MISC_EXPORT CDbInfoList :
        public CSequence<CDbInfo>,
        public swift::misc::db::IDatastoreObjectList<CDbInfo, CDbInfoList, int>,
        public swift::misc::mixin::MetaType<CDbInfoList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CDbInfoList)
        using CSequence::CSequence;

        //! Default constructor.
        CDbInfoList();

        //! Construct from a base class object.
        CDbInfoList(const CSequence<CDbInfo> &other);

        //! Find by entity
        CDbInfo findFirstByEntityOrDefault(swift::misc::network::CEntityFlags::Entity entity) const;

        //! From our database JSON format
        static CDbInfoList fromDatabaseJson(const QJsonArray &array);
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::db::CDbInfoList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::db::CDbInfo>)

#endif
