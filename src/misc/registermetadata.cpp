// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/registermetadata.h"
#include "misc/simulation/registermetadatasimulation.h"
#include "misc/weather/registermetadataweather.h"
#include "misc/network/registermetadatanetwork.h"
#include "misc/db/registermetadatadb.h"
#include "misc/audio/registermetadataaudio.h"
#include "misc/aviation/registermetadataaviation.h"
#include "misc/input/registermetadatainput.h"
#include "misc/geo/registermetadatageo.h"
#include "misc/pq/registermetadatapq.h"

#include "misc/sharedstate/passiveobserver.h"
#include "misc/applicationinfolist.h"
#include "misc/countrylist.h"
#include "misc/directories.h"
#include "misc/iconlist.h"
#include "misc/identifierlist.h"
#include "misc/logcategorylist.h"
#include "misc/logpattern.h"
#include "misc/namevariantpair.h"
#include "misc/namevariantpairlist.h"
#include "misc/pixmap.h"
#include "misc/platformset.h"
#include "misc/processinfo.h"
#include "misc/propertyindexlist.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/rgbcolor.h"
#include "misc/sequence.h"
#include "misc/statusmessagelist.h"
#include "misc/valuecache.h"
#include "misc/variantlist.h"
#include "misc/variantmap.h"

#include <QDBusMetaType>
#include <QMetaType>
#include <QtGlobal>

void initMiscResourcesImpl()
{
    // cannot be declared within namespace, see docu
    Q_INIT_RESOURCE(misc);
}

namespace swift::misc
{
    void registerMetadata()
    {
        initMiscResourcesImpl();

        CApplicationInfo::registerMetadata();
        CApplicationInfoList::registerMetadata();
        CCountry::registerMetadata();
        CCountryList::registerMetadata();
        CDirectories::registerMetadata();
        CIcon::registerMetadata();
        CIconList::registerMetadata();
        CIdentifier::registerMetadata();
        CIdentifierList::registerMetadata();
        CLogCategory::registerMetadata();
        CLogCategoryList::registerMetadata();
        CLogPattern::registerMetadata();
        CNameVariantPair::registerMetadata();
        CNameVariantPairList::registerMetadata();
        CPixmap::registerMetadata();
        CPlatform::registerMetadata();
        CPlatformSet::registerMetadata();
        CProcessInfo::registerMetadata();
        CPropertyIndex::registerMetadata();
        CPropertyIndex::registerMetadata();
        CPropertyIndexList::registerMetadata();
        CPropertyIndexVariantMap::registerMetadata();
        CRgbColor::registerMetadata();
        CStatusMessage::registerMetadata();
        CStatusMessageList::registerMetadata();
        CStrongStringView::registerMetadata();
        CValueCachePacket::registerMetadata();
        CVariant::registerMetadata();
        CVariantList::registerMetadata();
        CVariantMap::registerMetadata();

        // sub namespaces
        audio::registerMetadata();
        aviation::registerMetadata();
        db::registerMetadata();
        geo::registerMetadata();
        input::registerMetadata();
        network::registerMetadata();
        physical_quantities::registerMetadata();
        simulation::registerMetadata();
        weather::registerMetadata();

        shared_state::CAnyMatch::registerMetadata();

        // needed by xswiftbus proxy class
        qDBusRegisterMetaType<CSequence<double>>();
    }

} // ns
