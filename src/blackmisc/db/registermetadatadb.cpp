/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/db/registermetadatadb.h"
#include "blackmisc/db/db.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Db
    {
        void registerMetadata()
        {
            CDbFlags::registerMetadata();
            CDbInfo::registerMetadata();
            CDbInfoList::registerMetadata();
            CArtifact::registerMetadata();
            CArtifactList::registerMetadata();
            CDistribution::registerMetadata();
            CDistributionList::registerMetadata();
            CUpdateInfo::registerMetadata();
        }
    }

#if defined(Q_OS_WIN) && defined(Q_CC_CLANG)
    namespace Private
    {
        template void maybeRegisterMetaListConvert<Db::CDbInfoList>(int);
        template void maybeRegisterMetaListConvert<Db::CArtifactList>(int);
        template void maybeRegisterMetaListConvert<Db::CDistributionList>(int);
        template void maybeRegisterMetaListConvert<CSequence<Db::CDbInfo>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Db::CArtifact>>(int);
        template void maybeRegisterMetaListConvert<CSequence<Db::CDistribution>>(int);
    } // ns
#endif

} // ns
