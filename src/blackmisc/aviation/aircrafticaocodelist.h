/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTICAOCODELIST_H
#define BLACKMISC_AVIATION_AIRCRAFTICAOCODELIST_H

#include "aircrafticaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <initializer_list>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of ICAO codes.
        class BLACKMISC_EXPORT CAircraftIcaoCodeList :
            public CSequence<CAircraftIcaoCode>,
            public BlackMisc::Mixin::MetaType<CAircraftIcaoCodeList>
        {

        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftIcaoCodeList)

            //! Default constructor.
            CAircraftIcaoCodeList() = default;

            //! Construct from a base class object.
            CAircraftIcaoCodeList(const CSequence<CAircraftIcaoCode> &other);

            //! From our database JSON format
            static CAircraftIcaoCodeList fromDatabaseJson(const QJsonArray &array);
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcaoCodeList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftIcaoCode>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftIcaoCode>)

#endif //guard
