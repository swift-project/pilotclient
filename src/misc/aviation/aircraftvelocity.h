// SPDX-FileCopyrightText: Copyright (C) 2021 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTVELOCITY_H
#define SWIFT_MISC_AVIATION_AIRCRAFTVELOCITY_H

#include "misc/pq/speed.h"
#include "misc/pq/angle.h"
#include "misc/pq/time.h"
#include "misc/valueobject.h"
#include "misc/propertyindexref.h"
#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftVelocity)

namespace swift::misc::aviation
{
    //! Velocity and angular velocity for 6DOF bodies
    class SWIFT_MISC_EXPORT CAircraftVelocity : public CValueObject<CAircraftVelocity>
    {
    public:
        //! Property index
        enum ColumnIndex
        {
            IndexVelocityX = CPropertyIndexRef::GlobalIndexCAircraftVelocity,
            IndexVelocityY,
            IndexVelocityZ,
            IndexPitchVelocity,
            IndexRollVelocity,
            IndexHeadingVelocity
        };

        //! Default constructor
        CAircraftVelocity();

        //! Constructor
        CAircraftVelocity(double x, double y, double z, physical_quantities::CSpeedUnit xyzUnit,
                          double pitch, double roll, double heading, physical_quantities::CAngleUnit pbhAngleUnit, physical_quantities::CTimeUnit pbhTimeUnit);

        //! @{
        //! Setter
        void setLinearVelocity(double x, double y, double z, physical_quantities::CSpeedUnit xyzUnit);
        void setAngularVelocity(double pitch, double roll, double heading, physical_quantities::CAngleUnit pbhAngleUnit, physical_quantities::CTimeUnit pbhTimeUnit);
        //! @}

        //! @{
        //! Getter
        double getVelocityX(physical_quantities::CSpeedUnit unit) const;
        double getVelocityY(physical_quantities::CSpeedUnit unit) const;
        double getVelocityZ(physical_quantities::CSpeedUnit unit) const;
        double getPitchVelocity(physical_quantities::CAngleUnit angleUnit, physical_quantities::CTimeUnit timeUnit) const;
        double getRollVelocity(physical_quantities::CAngleUnit angleUnit, physical_quantities::CTimeUnit timeUnit) const;
        double getHeadingVelocity(physical_quantities::CAngleUnit angleUnit, physical_quantities::CTimeUnit timeUnit) const;
        //! @}

        //! \copydoc mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftVelocity &compareValue) const;

        //! @{
        //! Unit constants
        static const physical_quantities::CSpeedUnit c_xyzSpeedUnit;
        static const physical_quantities::CLengthUnit c_xyzLengthUnit;
        static const physical_quantities::CAngleUnit c_pbhAngleUnit; // TODO Create a dedicated PQ class for angular velocity
        static const physical_quantities::CTimeUnit c_timeUnit;
        //! @}

    private:
        double m_x = 0; //!< Positive moves east
        double m_y = 0; //!< Positive moves up
        double m_z = 0; //!< Positive moves north
        double m_pitch = 0;
        double m_roll = 0;
        double m_heading = 0;

        SWIFT_METACLASS(
            CAircraftVelocity,
            SWIFT_METAMEMBER(x),
            SWIFT_METAMEMBER(y),
            SWIFT_METAMEMBER(z),
            SWIFT_METAMEMBER(pitch),
            SWIFT_METAMEMBER(roll),
            SWIFT_METAMEMBER(heading));
    };
}

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftVelocity)

#endif
