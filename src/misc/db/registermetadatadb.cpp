// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/db/registermetadatadb.h"

#include "misc/variant.h"

// DB headers
#include "misc/db/artifact.h"
#include "misc/db/artifactlist.h"
#include "misc/db/dbflags.h"
#include "misc/db/dbinfo.h"
#include "misc/db/dbinfolist.h"
#include "misc/db/distribution.h"
#include "misc/db/distributionlist.h"
#include "misc/db/updateinfo.h"

namespace swift::misc::db
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

} // namespace swift::misc::db
