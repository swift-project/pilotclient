/* Copyright (C) 2021
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftvelocity.h"
#include "blackmisc/verify.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        const CSpeedUnit CAircraftVelocity::c_xyzUnit = CSpeedUnit::m_s();
        const CAngleUnit CAircraftVelocity::c_pbhAngleUnit = CAngleUnit::rad();
        const CTimeUnit CAircraftVelocity::c_pbhTimeUnit = CTimeUnit::s();

        CAircraftVelocity::CAircraftVelocity() = default;

        CAircraftVelocity::CAircraftVelocity(double x, double y, double z, CSpeedUnit xyzUnit, double pitch, double roll, double heading, CAngleUnit pbhAngleUnit, CTimeUnit pbhTimeUnit)
        {
            setLinearVelocity(x, y, z, xyzUnit);
            setAngularVelocity(pitch, roll, heading, pbhAngleUnit, pbhTimeUnit);
        }

        void CAircraftVelocity::setLinearVelocity(double x, double y, double z, CSpeedUnit xyzUnit)
        {
            m_x = c_xyzUnit.convertFrom(x, xyzUnit);
            m_y = c_xyzUnit.convertFrom(y, xyzUnit);
            m_z = c_xyzUnit.convertFrom(z, xyzUnit);
        }

        void CAircraftVelocity::setAngularVelocity(double pitch, double roll, double heading, CAngleUnit pbhAngleUnit, CTimeUnit pbhTimeUnit)
        {
            m_pitch = pbhTimeUnit.convertFrom(c_pbhAngleUnit.convertFrom(pitch, pbhAngleUnit), c_pbhTimeUnit);
            m_roll = pbhTimeUnit.convertFrom(c_pbhAngleUnit.convertFrom(roll, pbhAngleUnit), c_pbhTimeUnit);
            m_heading = pbhTimeUnit.convertFrom(c_pbhAngleUnit.convertFrom(heading, pbhAngleUnit), c_pbhTimeUnit);
        }

        double CAircraftVelocity::getVelocityX(CSpeedUnit unit) const
        {
            return unit.convertFrom(m_x, c_xyzUnit);
        }

        double CAircraftVelocity::getVelocityY(CSpeedUnit unit) const
        {
            return unit.convertFrom(m_y, c_xyzUnit);
        }

        double CAircraftVelocity::getVelocityZ(CSpeedUnit unit) const
        {
            return unit.convertFrom(m_z, c_xyzUnit);
        }

        double CAircraftVelocity::getPitchVelocity(CAngleUnit angleUnit, CTimeUnit timeUnit) const
        {
            return c_pbhTimeUnit.convertFrom(angleUnit.convertFrom(m_pitch, c_pbhAngleUnit), timeUnit);
        }

        double CAircraftVelocity::getRollVelocity(CAngleUnit angleUnit, CTimeUnit timeUnit) const
        {
            return c_pbhTimeUnit.convertFrom(angleUnit.convertFrom(m_roll, c_pbhAngleUnit), timeUnit);
        }

        double CAircraftVelocity::getHeadingVelocity(CAngleUnit angleUnit, CTimeUnit timeUnit) const
        {
            return c_pbhTimeUnit.convertFrom(angleUnit.convertFrom(m_heading, c_pbhAngleUnit), timeUnit);
        }

        QString CAircraftVelocity::convertToQString(bool i18n) const
        {
            //! \todo Implement CAircraftVelocity::convertToQString
            Q_UNUSED(i18n);
            return QStringLiteral("velocity{}");
        }

        QVariant CAircraftVelocity::propertyByIndex(CPropertyIndexRef index) const
        {
            BLACK_VERIFY(index.isMyself());
            return QVariant::fromValue(*this);
        }

        void CAircraftVelocity::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            BLACK_VERIFY(index.isMyself());
            *this = variant.value<CAircraftVelocity>();
        }

        int CAircraftVelocity::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftVelocity &compareValue) const
        {
            BLACK_VERIFY(index.isMyself());
            return compare(*this, compareValue);
        }
    }
}
