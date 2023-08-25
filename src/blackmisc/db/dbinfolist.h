// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_DB_DBINFOYLIST_H
#define BLACKMISC_DB_DBINFOYLIST_H

#include "blackmisc/db/dbinfo.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/blackmiscexport.h"

#include <initializer_list>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Db, CDbInfo, CDbInfoList)

namespace BlackMisc::Db
{
    //! Value object encapsulating a list of info objects.
    class BLACKMISC_EXPORT CDbInfoList :
        public CSequence<CDbInfo>,
        public BlackMisc::Db::IDatastoreObjectList<CDbInfo, CDbInfoList, int>,
        public BlackMisc::Mixin::MetaType<CDbInfoList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDbInfoList)
        using CSequence::CSequence;

        //! Default constructor.
        CDbInfoList();

        //! Construct from a base class object.
        CDbInfoList(const CSequence<CDbInfo> &other);

        //! Find by entity
        CDbInfo findFirstByEntityOrDefault(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! From our database JSON format
        static CDbInfoList fromDatabaseJson(const QJsonArray &array);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Db::CDbInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Db::CDbInfo>)

#endif // guard
