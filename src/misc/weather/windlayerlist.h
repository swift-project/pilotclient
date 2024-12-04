// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_WINDLAYERLIST_H
#define SWIFT_MISC_WEATHER_WINDLAYERLIST_H

#include <initializer_list>
#include <tuple>

#include <QMetaType>

#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/weather/windlayer.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::weather, CWindLayer, CWindLayerList)

namespace swift::misc
{
    namespace aviation
    {
        class CAltitude;
    }

    namespace weather
    {
        /*!
         * Value object encapsulating a set of wind layers
         */
        class SWIFT_MISC_EXPORT CWindLayerList :
            public CSequence<CWindLayer>,
            public swift::misc::mixin::MetaType<CWindLayerList>
        {
        public:
            SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CWindLayerList)
            using CSequence::CSequence;

            //! Default constructor.
            CWindLayerList() = default;

            //! Construct from a base class object.
            CWindLayerList(const CSequence<CWindLayer> &other);

            //! Contains cloud layer with level?
            bool containsLevel(const swift::misc::aviation::CAltitude &level) const;

            //! Find cloud layer by level
            CWindLayer findByLevel(const swift::misc::aviation::CAltitude &level) const;
        };

    } // namespace weather
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::weather::CWindLayerList)

#endif // SWIFT_MISC_WEATHER_WINDLAYERLIST_H
