/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/simulation/xplane/navdatareference.h"

#include <QtGlobal>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
        {
            CNavDataReference::CNavDataReference() = default;

            CNavDataReference::CNavDataReference(int id, const Geo::CLatitude &latitude, const Geo::CLongitude &longitude) :
                m_id(id),
                m_position(latitude, longitude, {})
            {}

            CNavDataReference::CNavDataReference(int id, float latitudeDegrees, float longitudeDegrees) :
                m_id(id),
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
                if (index.isMyself()) { (*this) = variant.value<CNavDataReference>(); return; }
                CValueObject::setPropertyByIndex(index, variant);
            }

            QString CNavDataReference::convertToQString(bool i18n) const
            {
                return QString::number(id()) + ":" + m_position.convertToQString(i18n);
            }

            CNavDataReferenceList::CNavDataReferenceList() = default;

            CNavDataReferenceList::CNavDataReferenceList(const CSequence<CNavDataReference> &other) :
                CSequence<CNavDataReference>(other)
            {}
        }
    }
}
