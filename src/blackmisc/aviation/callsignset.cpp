/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/containerbase.h"
#include "blackmisc/dbus.h"
#include "blackmisc/variant.h"

#include <QDBusMetaType>
#include <QJsonObject>
#include <QString>

namespace BlackMisc
{
    namespace Aviation
    {
        CCallsignSet::CCallsignSet() { }

        CCallsignSet::CCallsignSet(const CCollection<CCallsign> &other) :
            CCollection<CCallsign>(other)
        { }

        bool CCallsignSet::containsCallsign(const QString &callsign) const
        {
            return this->contains(CCallsign(callsign));
        }

        QStringList CCallsignSet::getCallsignStrings(bool sorted) const
        {
            QStringList callsigns;
            for (const CCallsign &cs : *this)
            {
                callsigns.push_back(cs.asString());
            }
            if (sorted) { callsigns.sort(); }
            return callsigns;
        }

        void CCallsignSet::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CCallsign>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CCallsign>>();
            qRegisterMetaType<BlackMisc::CCollection<CCallsign>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CCallsign>>();
            qRegisterMetaType<CCallsignSet>();
            qDBusRegisterMetaType<CCallsignSet>();
            registerMetaValueType<CCallsignSet>();
        }
    } // namespace
} // namespace
