/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_VISIBILITYLAYERLIST_H
#define BLACKMISC_WEATHER_VISIBILITYLAYERLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/visibilitylayer.h"

#include <QMetaType>
#include <initializer_list>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CVisibilityLayer, CVisibilityLayerList)

namespace BlackMisc
{
    namespace Aviation
    {
        class CAltitude;
    }

    namespace Weather
    {
        /*!
         * Value object encapsulating a set of visibility layers
         */
        class BLACKMISC_EXPORT CVisibilityLayerList :
            public CSequence<CVisibilityLayer>,
            public BlackMisc::Mixin::MetaType<CVisibilityLayerList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CVisibilityLayerList)
            using CSequence::CSequence;

            //! Default constructor.
            CVisibilityLayerList() = default;

            //! Construct from a base class object.
            CVisibilityLayerList(const CSequence<CVisibilityLayer> &other);

            //! Contains visibility layer with base?
            bool containsBase(const BlackMisc::Aviation::CAltitude &base) const;

            //! Find visibility layer by base
            CVisibilityLayer findByBase(const BlackMisc::Aviation::CAltitude &base) const;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CVisibilityLayerList)

#endif // guard
