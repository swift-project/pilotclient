/* Copyright (C) 2021
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTVELOCITY_H
#define BLACKMISC_AVIATION_AIRCRAFTVELOCITY_H

#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/blackmiscexport.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAircraftVelocity)

namespace BlackMisc::Aviation
{
    //! Velocity and angular velocity for 6DOF bodies
    class BLACKMISC_EXPORT CAircraftVelocity : public CValueObject<CAircraftVelocity>
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
        CAircraftVelocity(double x, double y, double z, PhysicalQuantities::CSpeedUnit xyzUnit,
            double pitch, double roll, double heading, PhysicalQuantities::CAngleUnit pbhAngleUnit, PhysicalQuantities::CTimeUnit pbhTimeUnit);

        //! @{
        //! Setter
        void setLinearVelocity(double x, double y, double z, PhysicalQuantities::CSpeedUnit xyzUnit);
        void setAngularVelocity(double pitch, double roll, double heading, PhysicalQuantities::CAngleUnit pbhAngleUnit, PhysicalQuantities::CTimeUnit pbhTimeUnit);
        //! @}

        //! @{
        //! Getter
        double getVelocityX(PhysicalQuantities::CSpeedUnit unit) const;
        double getVelocityY(PhysicalQuantities::CSpeedUnit unit) const;
        double getVelocityZ(PhysicalQuantities::CSpeedUnit unit) const;
        double getPitchVelocity(PhysicalQuantities::CAngleUnit angleUnit, PhysicalQuantities::CTimeUnit timeUnit) const;
        double getRollVelocity(PhysicalQuantities::CAngleUnit angleUnit, PhysicalQuantities::CTimeUnit timeUnit) const;
        double getHeadingVelocity(PhysicalQuantities::CAngleUnit angleUnit, PhysicalQuantities::CTimeUnit timeUnit) const;
        //! @}

        //! \copydoc Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftVelocity &compareValue) const;

        //! @{
        //! Unit constants
        static const PhysicalQuantities::CSpeedUnit c_xyzSpeedUnit;
        static const PhysicalQuantities::CLengthUnit c_xyzLengthUnit;
        static const PhysicalQuantities::CAngleUnit c_pbhAngleUnit; // TODO Create a dedicated PQ class for angular velocity
        static const PhysicalQuantities::CTimeUnit c_timeUnit;
        //! @}

    private:
        double m_x = 0; //!< Positive moves east
        double m_y = 0; //!< Positive moves up
        double m_z = 0; //!< Positive moves north
        double m_pitch = 0;
        double m_roll = 0;
        double m_heading = 0;

        BLACK_METACLASS(
            CAircraftVelocity,
            BLACK_METAMEMBER(x),
            BLACK_METAMEMBER(y),
            BLACK_METAMEMBER(z),
            BLACK_METAMEMBER(pitch),
            BLACK_METAMEMBER(roll),
            BLACK_METAMEMBER(heading)
        );
    };
}

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftVelocity)

#endif
