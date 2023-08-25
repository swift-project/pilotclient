// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/db/registermetadatadb.h"
#include "blackmisc/variant.h"

// DB headers
#include "blackmisc/db/dbinfolist.h"
#include "blackmisc/db/dbinfo.h"
#include "blackmisc/db/dbflags.h"
#include "blackmisc/db/distribution.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/db/artifact.h"
#include "blackmisc/db/artifactlist.h"
#include "blackmisc/db/updateinfo.h"

namespace BlackMisc::Db
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

} // ns
