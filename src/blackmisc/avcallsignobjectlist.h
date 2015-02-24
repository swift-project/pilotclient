/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CALLSIGNOBJECTLIST_H
#define BLACKMISC_CALLSIGNOBJECTLIST_H

#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/avcallsignlist.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/propertyindexvariantmap.h"
#include <QList>
#include <QHash>

namespace BlackMisc
{
    namespace Aviation
    {
        //! List of objects with callsign.
        template<class OBJ, class CONTAINER>
        class ICallsignObjectList
        {
        public:

            //! Contains callsign?
            bool containsCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Apply for given callsign
            int applyIfCallsign(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CPropertyIndexVariantMap &variantMap);

            //! Find 0..n stations by callsign
            CONTAINER findByCallsign(const CCallsign &callsign) const;

            //! Find 0..n aircraft matching any of a set of callsigns
            CONTAINER findByCallsigns(const CCallsignList &callsigns) const;

            //! Find the first aircraft by callsign, if none return given one
            OBJ findFirstByCallsign(const CCallsign &callsign, const OBJ &ifNotFound = {}) const;

            //! Find the back object by callsign, if none return given one
            OBJ findBackByCallsign(const CCallsign &callsign, const OBJ &ifNotFound = {}) const;

            //! All with given suffix, empty suffixes ignored
            CONTAINER findBySuffix(const QString &suffix) const;

            //! Remove all objects with callsign
            int removeByCallsign(const CCallsign &callsign);

            //! All suffixes with their respective count
            QMap<QString, int> getSuffixes() const;

            //! Split into 0..n containers as per callsign
            QHash<BlackMisc::Aviation::CCallsign, CONTAINER> splitPerCallsign() const;

            //! Sort by callsign
            void sortByCallsign();

            //! Incremental update or add object
            int incrementalUpdateOrAdd(const OBJ &objectBeforeChanged, const BlackMisc::CPropertyIndexVariantMap &changedValues);

        protected:
            //! Constructor
            ICallsignObjectList();

            //! Container
            virtual const CONTAINER &container() const = 0;

            //! Container
            virtual CONTAINER &container() = 0;
        };

    } //namespace
} // namespace

#endif //guard
