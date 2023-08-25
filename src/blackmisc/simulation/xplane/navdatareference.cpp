// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/simulation/xplane/navdatareference.h"

#include <QtGlobal>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Simulation::XPlane, CNavDataReference)
BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Simulation::XPlane, CNavDataReference, CNavDataReferenceList)

namespace BlackMisc::Simulation::XPlane
{
    CNavDataReference::CNavDataReference() = default;

    CNavDataReference::CNavDataReference(int id, const Geo::CLatitude &latitude, const Geo::CLongitude &longitude) : m_id(id),
                                                                                                                     m_position(latitude, longitude, {})
    {}

    CNavDataReference::CNavDataReference(int id, float latitudeDegrees, float longitudeDegrees) : m_id(id),
                                                                                                  m_position(latitudeDegrees, longitudeDegrees, 0)
    {}

    QVariant CNavDataReference::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        return ICoordinateGeodetic::canHandleIndex(index) ?
                   ICoordinateGeodetic::propertyByIndex(index) :
                   CValueObject::propertyByIndex(index);
    }

    void CNavDataReference::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CNavDataReference>();
            return;
        }
        CValueObject::setPropertyByIndex(index, variant);
    }

    QString CNavDataReference::convertToQString(bool i18n) const
    {
        return QString::number(id()) + ":" + m_position.convertToQString(i18n);
    }

    CNavDataReferenceList::CNavDataReferenceList() = default;

    CNavDataReferenceList::CNavDataReferenceList(const CSequence<CNavDataReference> &other) : CSequence<CNavDataReference>(other)
    {}
}
