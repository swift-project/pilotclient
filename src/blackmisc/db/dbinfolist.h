/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DBINFOYLIST_H
#define BLACKMISC_DB_DBINFOYLIST_H

#include "dbinfo.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/blackmiscexport.h"

#include <initializer_list>

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
} //namespace

Q_DECLARE_METATYPE(BlackMisc::Db::CDbInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Db::CDbInfo>)

#endif //guard
