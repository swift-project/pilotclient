/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_XPLANE_NAVDATAREFERENCE_H
#define BLACKMISC_SIMULATION_XPLANE_NAVDATAREFERENCE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/sequence.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>
#include <tuple>

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
                virtual const Aviation::CAltitude &geodeticHeight() const override { return m_position.geodeticHeight(); }
                virtual QVector3D normalVector() const override { return m_position.normalVector(); }
                virtual std::array<double, 3> normalVectorDouble() const override { return this->m_position.normalVectorDouble(); }

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

            private:
                int m_id = 0;
                Geo::CCoordinateGeodetic m_position;

                BLACK_METACLASS(
                    CNavDataReference,
                    BLACK_METAMEMBER(id),
                    BLACK_METAMEMBER(position)
                );
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
                using CSequence::CSequence;

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
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::XPlane::CNavDataReference>)

#endif
