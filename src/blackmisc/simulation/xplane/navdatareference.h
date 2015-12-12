/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_XPLANE_NAVDATAREFERENCE_H
#define BLACKMISC_SIMULATION_XPLANE_NAVDATAREFERENCE_H

#include "blackmisc/sequence.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/geo/coordinategeodetic.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
        {
            /*!
             * Reference to an entry in X-Plane's navigation data (airport, navaid, fix, etc.) with its position.
             */
            class BLACKMISC_EXPORT CNavDataReference : public CValueObject<CNavDataReference>, public Geo::ICoordinateGeodetic
            {
            public:
                //! Default constructor.
                CNavDataReference();

                //! Construct from a navdata reference ID and position.
                CNavDataReference(int id, const Geo::CLatitude &latitude, const Geo::CLongitude &longitude);

                //! Construct from a navdata reference ID and position.
                CNavDataReference(int id, float latitudeDegrees, float longitudeDegrees);

                //! Return the navdata reference ID.
                int id() const { return m_id; }

                virtual Geo::CLatitude latitude() const override { return m_position.latitude(); }
                virtual Geo::CLongitude longitude() const override { return m_position.longitude(); }
                virtual const PhysicalQuantities::CLength &geodeticHeight() const override { return m_position.geodeticHeight(); }
                virtual QVector3D normalVector() const override { return m_position.normalVector(); }
                virtual std::array<double, 3> normalVectorDouble() const override { return this->m_position.normalVectorDouble(); }

                //! \copydoc CValueObject::propertyByIndex
                CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

                //! \copydoc CValueObject::setPropertyByIndex
                void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

                //! \copydoc CValueObject::convertToQString
                QString convertToQString(bool i18n = false) const;

            private:
                BLACK_ENABLE_TUPLE_CONVERSION(CNavDataReference)
                int m_id = 0;
                Geo::CCoordinateGeodetic m_position;
            };

            /*!
             * List of X-Plane navigation data entries (airports, navaids, fixes, etc) with their positions.
             */
            class BLACKMISC_EXPORT CNavDataReferenceList :
                public CSequence<CNavDataReference>,
                public Geo::IGeoObjectList<CNavDataReference, CNavDataReferenceList>,
                public Mixin::MetaType<CNavDataReferenceList>
            {
            public:
                BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CNavDataReferenceList)

                //! Default constructor.
                CNavDataReferenceList();

                //! Construct from a base class object.
                CNavDataReferenceList(const CSequence<CNavDataReference> &other);
            };
        }
    }
}

Q_DECLARE_METATYPE(BlackMisc::Simulation::XPlane::CNavDataReference)
Q_DECLARE_METATYPE(BlackMisc::Simulation::XPlane::CNavDataReferenceList)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::XPlane::CNavDataReference>)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::XPlane::CNavDataReference>)

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::XPlane::CNavDataReference, (
    attr(o.m_id),
    attr(o.m_position)
))

#endif
