/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_CLOUDLAYERLIST_H
#define BLACKMISC_WEATHER_CLOUDLAYERLIST_H

#include "cloudlayer.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"

namespace BlackMisc
{
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

            //! Default constructor.
            CCloudLayerList() = default;

            //! Construct from a base class object.
            CCloudLayerList(const CSequence<CCloudLayer> &other);

            //! Contains cloud layer with ceiling?
            bool containsCeiling(const BlackMisc::Aviation::CAltitude &ceiling) const;

            //! Find cloud layer by ceiling
            CCloudLayer findByCeiling(const BlackMisc::Aviation::CAltitude &ceiling) const;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayerList)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Weather::CCloudLayer>)

#endif //guard
