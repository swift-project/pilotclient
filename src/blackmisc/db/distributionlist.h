/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DB_DISTRIBUTIONLIST_H
#define BLACKMISC_DB_DISTRIBUTIONLIST_H

#include "distribution.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/datacache.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Db
    {
        //! Value object encapsulating a list of aircraft models
        class BLACKMISC_EXPORT CDistributionList :
            public BlackMisc::CSequence<CDistribution>,
            public BlackMisc::Db::IDatastoreObjectList<CDistribution, CDistributionList, int>,
            public BlackMisc::Mixin::MetaType<CDistributionList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CDistributionList)

            //! Empty constructor.
            CDistributionList();

            //! Construct from a base class object.
            CDistributionList(const CSequence<CDistribution> &other);

            //! All channels
            QStringList getChannels() const;

            //! Find distribution by channels
            CDistribution findByChannelOrDefault(const QString &channel) const;

            //! From database JSON
            static CDistributionList fromDatabaseJson(const QJsonArray &array);
        };

        //! Trait for global setup data
        struct TDistributionInfo : public BlackMisc::TDataTrait<CDistributionList>
        {
            //! Key in data cache
            static const char *key() { return "distributions"; }

            //! First load is synchronous
            static constexpr bool isPinned() { return true; }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Db::CDistributionList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Db::CDistribution>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Db::CDistribution>)

#endif //guard
