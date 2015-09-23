/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/predicates.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {

        CLiveryList::CLiveryList() { }

        CLiveryList::CLiveryList(const CSequence<CLivery> &other) :
            CSequence<CLivery>(other)
        { }

        CLiveryList CLiveryList::findByAirlineIcaoDesignator(const QString &icao) const
        {
            QString i(icao.trimmed().toUpper());
            if (i.isEmpty()) { return CLiveryList(); }
            return this->findBy(&CLivery::getAirlineIcaoCodeDesignator, i);
        }

        CLivery CLiveryList::findByAirlineIcaoDesignatorStdLivery(const QString &icao) const
        {
            QString i(icao.trimmed().toUpper());
            if (i.isEmpty()) { return CLivery(); }
            return this->findFirstByOrDefault([&](const CLivery & livery)
            {
                return livery.getAirlineIcaoCodeDesignator() == icao &&
                       livery.isAirlineStandardLivery();

            });
        }

        CLivery CLiveryList::findByCombinedCode(const QString &combinedCode) const
        {
            if (!CLivery::isValidCombinedCode(combinedCode)) { return CLivery(); }
            return this->findFirstByOrDefault([&](const CLivery & livery)
            {
                return livery.matchesCombinedCode(combinedCode);
            });
        }

    } // namespace
} // namespace
