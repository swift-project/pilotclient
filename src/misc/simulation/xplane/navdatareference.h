// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_XPLANE_NAVDATAREFERENCE_H
#define SWIFT_MISC_SIMULATION_XPLANE_NAVDATAREFERENCE_H

#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/geoobjectlist.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/metaclass.h"
#include "misc/pq/length.h"
#include "misc/propertyindexref.h"
#include "misc/sequence.h"
#include "misc/valueobject.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::XPlane, CNavDataReference)
BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc::simulation::XPlane, CNavDataReference, CNavDataReferenceList)

namespace swift::misc::simulation::xplane
{
    /*!
     * Reference to an entry in X-Plane's navigation data (airport, navaid, fix, etc.) with its position.
     */
    class SWIFT_MISC_EXPORT CNavDataReference : public CValueObject<CNavDataReference>, public geo::ICoordinateGeodetic
    {
    public:
        //! Default constructor.
        CNavDataReference();

        //! Construct from a navdata reference ID and position.
        CNavDataReference(int id, const geo::CLatitude &latitude, const geo::CLongitude &longitude);

        //! Construct from a navdata reference ID and position.
        CNavDataReference(int id, float latitudeDegrees, float longitudeDegrees);

        //! Return the navdata reference ID.
        int id() const { return m_id; }

        virtual geo::CLatitude latitude() const override { return m_position.latitude(); }
        virtual geo::CLongitude longitude() const override { return m_position.longitude(); }
        virtual const aviation::CAltitude &geodeticHeight() const override { return m_position.geodeticHeight(); }
        virtual QVector3D normalVector() const override { return m_position.normalVector(); }
        virtual std::array<double, 3> normalVectorDouble() const override { return this->m_position.normalVectorDouble(); }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        int m_id = 0;
        geo::CCoordinateGeodetic m_position;

        BLACK_METACLASS(
            CNavDataReference,
            BLACK_METAMEMBER(id),
            BLACK_METAMEMBER(position)
        );
    };

    /*!
     * List of X-Plane navigation data entries (airports, navaids, fixes, etc) with their positions.
     */
    class SWIFT_MISC_EXPORT CNavDataReferenceList :
        public CSequence<CNavDataReference>,
        public geo::IGeoObjectList<CNavDataReference, CNavDataReferenceList>,
        public mixin::MetaType<CNavDataReferenceList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CNavDataReferenceList)
        using CSequence::CSequence;

        //! Default constructor.
        CNavDataReferenceList();

        //! Construct from a base class object.
        CNavDataReferenceList(const CSequence<CNavDataReference> &other);
    };
}

Q_DECLARE_METATYPE(swift::misc::simulation::xplane::CNavDataReference)
Q_DECLARE_METATYPE(swift::misc::simulation::xplane::CNavDataReferenceList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::xplane::CNavDataReference>)

#endif
