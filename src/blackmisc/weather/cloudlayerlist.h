/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
    namespace Aviation { class CAltitude; }

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

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayerList)

#endif //guard
