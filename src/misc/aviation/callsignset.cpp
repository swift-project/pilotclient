// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/callsignset.h"
#include "misc/containerbase.h"

#include <QDBusMetaType>
#include <QString>

BLACK_DEFINE_COLLECTION_MIXINS(swift::misc::aviation, CCallsign, CCallsignSet)

namespace swift::misc::aviation
{
    CCallsignSet::CCallsignSet() {}

    CCallsignSet::CCallsignSet(const QStringList &callsigns, CCallsign::TypeHint typeHint)
    {
        for (const QString &c : callsigns)
        {
            if (c.isEmpty()) { continue; }
            const CCallsign cs = CCallsign(c, typeHint);
            this->push_back(cs);
        }
    }

    CCallsignSet::CCallsignSet(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        this->push_back(callsign);
    }

    CCallsignSet::CCallsignSet(const CCollection<CCallsign> &other) : CCollection<CCallsign>(other)
    {}

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

    QString CCallsignSet::getCallsignsAsString(bool sorted, const QString &separator) const
    {
        if (this->isEmpty()) { return {}; }
        return this->getCallsignStrings(sorted).join(separator);
    }

    void CCallsignSet::registerMetadata()
    {
        qRegisterMetaType<swift::misc::CSequence<CCallsign>>();
        qDBusRegisterMetaType<swift::misc::CSequence<CCallsign>>();
        qRegisterMetaType<swift::misc::CCollection<CCallsign>>();
        qDBusRegisterMetaType<swift::misc::CCollection<CCallsign>>();
        qRegisterMetaType<CCallsignSet>();
        qDBusRegisterMetaType<CCallsignSet>();
        registerMetaValueType<CCallsignSet>();
    }
} // namespace
