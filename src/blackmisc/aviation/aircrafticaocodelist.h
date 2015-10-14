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
#include "blackmisc/datastoreobjectlist.h"
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
            public BlackMisc::IDatastoreObjectListWithIntegerKey<CAircraftIcaoCode, CAircraftIcaoCodeList>,
            public BlackMisc::Mixin::MetaType<CAircraftIcaoCodeList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftIcaoCodeList)

            //! Default constructor.
            CAircraftIcaoCodeList() = default;

            //! Construct from a base class object.
            CAircraftIcaoCodeList(const CSequence<CAircraftIcaoCode> &other);

            //! Find by designator
            CAircraftIcaoCodeList findByDesignator(const QString &designator);

            //! Find by manufacturer
            CAircraftIcaoCodeList findByManufacturer(const QString &manufacturer);

            //! Find by model description
            CAircraftIcaoCodeList findByDescription(const QString &description);

            //! Find by designator, then best match by rank
            CAircraftIcaoCode findFirstByDesignatorAndRank(const QString &designator);

            //! Best selection by given pattern
            CAircraftIcaoCode smartAircraftIcaoSelector(const CAircraftIcaoCode &icaoPattern) const;

            //! Sort by rank
            void sortByRank();

            //! For selection completion
            QStringList toCompleterStrings() const;

            //! From our database JSON format
            static CAircraftIcaoCodeList fromDatabaseJson(const QJsonArray &array, bool ignoreIncomplete = true);
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftIcaoCodeList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftIcaoCode>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftIcaoCode>)

#endif //guard
