/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "registermetadata.h"
#include "audio/registermetadataaudio.h"
#include "aviation/registermetadataaviation.h"
#include "geo/registermetadatageo.h"
#include "input/registermetadatainput.h"
#include "network/registermetadatanetwork.h"
#include "pq/registermetadatapq.h"
#include "simulation/registermetadatasimulation.h"
#include "weather/registermetadataweather.h"
#include "propertyindexlist.h"
#include "propertyindexvariantmap.h"
#include "namevariantpairlist.h"
#include "variantlist.h"
#include "variantmap.h"
#include "valuecache.h"
#include "rgbcolor.h"
#include "countrylist.h"
#include "statusmessagelist.h"
#include "pixmap.h"
#include "iconlist.h"
#include "identifierlist.h"
#include "logpattern.h"

namespace BlackMisc
{
    void registerMetadata()
    {
        CCountry::registerMetadata();
        CCountryList::registerMetadata();
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
        CPropertyIndex::registerMetadata();
        CPropertyIndex::registerMetadata();
        CPropertyIndexList::registerMetadata();
        CPropertyIndexVariantMap::registerMetadata();
        CRgbColor::registerMetadata();
        CStatusMessage::registerMetadata();
        CStatusMessageList::registerMetadata();
        CValueCachePacket::registerMetadata();
        CVariant::registerMetadata();
        CVariantList::registerMetadata();
        CVariantMap::registerMetadata();

        // sub namespaces
        Audio::registerMetadata();
        Aviation::registerMetadata();
        Geo::registerMetadata();
        Input::registerMetadata();
        Network::registerMetadata();
        PhysicalQuantities::registerMetadata();
        Simulation::registerMetadata();
        Weather::registerMetadata();

        // needed by XBus proxy class
        qRegisterMetaType<CSequence<double>>();
        qRegisterMetaType<CSequence<double>>("CDoubleSequence");
        qDBusRegisterMetaType<CSequence<double>>();
    }
} // ns
