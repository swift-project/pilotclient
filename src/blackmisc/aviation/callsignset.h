/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_CALLSIGNSET_H
#define BLACKMISC_AVIATION_CALLSIGNSET_H

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/compare.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for a set of callsigns.
        class BLACKMISC_EXPORT CCallsignSet : public CCollection<CCallsign>
        {
        public:
            //! Default constructor.
            CCallsignSet();

            //! Initializer list constructor.
            CCallsignSet(std::initializer_list<CCallsign> il) : CCollection<CCallsign>(il) {}

            //! Construct from a base class object.
            CCallsignSet(const CCollection<CCallsign> &other);

            //! Contains by string
            bool containsCallsign(const QString &callsign) const;

            //! The callsign strings
            QStringList getCallsignStrings(bool sorted = false) const;

            //! Register metadata
            static void registerMetadata();
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CCallsignSet)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CCallsign>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CCallsign>)

#endif //guard
