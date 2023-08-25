// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_CLOUDLAYERLIST_H
#define BLACKMISC_WEATHER_CLOUDLAYERLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/cloudlayer.h"

#include <QMetaType>
#include <initializer_list>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CCloudLayer, CCloudLayerList)

namespace BlackMisc
{
    namespace Aviation
    {
        class CAltitude;
    }

    namespace Weather
    {
        /*!
         * Value object encapsulating a set of cloud layers
         */
        class BLACKMISC_EXPORT CCloudLayerList :
            public CSequence<CCloudLayer>,
            public BlackMisc::Mixin::MetaType<CCloudLayerList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CCloudLayerList)
            using CSequence::CSequence;

            //! Default constructor.
            CCloudLayerList() = default;

            //! Construct from a base class object.
            CCloudLayerList(const CSequence<CCloudLayer> &other);

            //! Contains cloud layer with base?
            bool containsBase(const BlackMisc::Aviation::CAltitude &base) const;

            //! Find cloud layer by base
            CCloudLayer findByBase(const BlackMisc::Aviation::CAltitude &base) const;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayerList)

#endif // guard
