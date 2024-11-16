// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_CLOUDLAYERLIST_H
#define SWIFT_MISC_WEATHER_CLOUDLAYERLIST_H

#include <initializer_list>
#include <tuple>

#include <QMetaType>

#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/weather/cloudlayer.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::weather, CCloudLayer, CCloudLayerList)

namespace swift::misc
{
    namespace aviation
    {
        class CAltitude;
    }

    namespace weather
    {
        /*!
         * Value object encapsulating a set of cloud layers
         */
        class SWIFT_MISC_EXPORT CCloudLayerList :
            public CSequence<CCloudLayer>,
            public mixin::MetaType<CCloudLayerList>
        {
        public:
            SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CCloudLayerList)
            using CSequence::CSequence;

            //! Default constructor.
            CCloudLayerList() = default;

            //! Construct from a base class object.
            CCloudLayerList(const CSequence<CCloudLayer> &other);

            //! Contains cloud layer with base?
            bool containsBase(const swift::misc::aviation::CAltitude &base) const;

            //! Find cloud layer by base
            CCloudLayer findByBase(const swift::misc::aviation::CAltitude &base) const;
        };

    } // namespace weather
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::weather::CCloudLayerList)

#endif // guard
