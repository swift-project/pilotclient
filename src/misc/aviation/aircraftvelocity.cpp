// SPDX-FileCopyrightText: Copyright (C) 2021 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftvelocity.h"
#include "misc/verify.h"
#include <QStringBuilder>

using namespace swift::misc::physical_quantities;

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftVelocity)

namespace swift::misc::aviation
{
    const CSpeedUnit CAircraftVelocity::c_xyzSpeedUnit = CSpeedUnit::m_s();
    const CLengthUnit CAircraftVelocity::c_xyzLengthUnit = CLengthUnit::m();
    const CAngleUnit CAircraftVelocity::c_pbhAngleUnit = CAngleUnit::rad();
    const CTimeUnit CAircraftVelocity::c_timeUnit = CTimeUnit::s();

    CAircraftVelocity::CAircraftVelocity() = default;

    CAircraftVelocity::CAircraftVelocity(double x, double y, double z, CSpeedUnit xyzUnit, double pitch, double roll, double heading, CAngleUnit pbhAngleUnit, CTimeUnit pbhTimeUnit)
    {
        setLinearVelocity(x, y, z, xyzUnit);
        setAngularVelocity(pitch, roll, heading, pbhAngleUnit, pbhTimeUnit);
    }

    void CAircraftVelocity::setLinearVelocity(double x, double y, double z, CSpeedUnit xyzUnit)
    {
        m_x = c_xyzSpeedUnit.convertFrom(x, xyzUnit);
        m_y = c_xyzSpeedUnit.convertFrom(y, xyzUnit);
        m_z = c_xyzSpeedUnit.convertFrom(z, xyzUnit);
    }

    void CAircraftVelocity::setAngularVelocity(double pitch, double roll, double heading, CAngleUnit pbhAngleUnit, CTimeUnit pbhTimeUnit)
    {
        m_pitch = pbhTimeUnit.convertFrom(c_pbhAngleUnit.convertFrom(pitch, pbhAngleUnit), c_timeUnit);
        m_roll = pbhTimeUnit.convertFrom(c_pbhAngleUnit.convertFrom(roll, pbhAngleUnit), c_timeUnit);
        m_heading = pbhTimeUnit.convertFrom(c_pbhAngleUnit.convertFrom(heading, pbhAngleUnit), c_timeUnit);
    }

    double CAircraftVelocity::getVelocityX(CSpeedUnit unit) const
    {
        return unit.convertFrom(m_x, c_xyzSpeedUnit);
    }

    double CAircraftVelocity::getVelocityY(CSpeedUnit unit) const
    {
        return unit.convertFrom(m_y, c_xyzSpeedUnit);
    }

    double CAircraftVelocity::getVelocityZ(CSpeedUnit unit) const
    {
        return unit.convertFrom(m_z, c_xyzSpeedUnit);
    }

    double CAircraftVelocity::getPitchVelocity(CAngleUnit angleUnit, CTimeUnit timeUnit) const
    {
        return c_timeUnit.convertFrom(angleUnit.convertFrom(m_pitch, c_pbhAngleUnit), timeUnit);
    }

    double CAircraftVelocity::getRollVelocity(CAngleUnit angleUnit, CTimeUnit timeUnit) const
    {
        return c_timeUnit.convertFrom(angleUnit.convertFrom(m_roll, c_pbhAngleUnit), timeUnit);
    }

    double CAircraftVelocity::getHeadingVelocity(CAngleUnit angleUnit, CTimeUnit timeUnit) const
    {
        return c_timeUnit.convertFrom(angleUnit.convertFrom(m_heading, c_pbhAngleUnit), timeUnit);
    }

    QString CAircraftVelocity::convertToQString(bool i18n) const
    {
        return u"Velocity: " % QStringLiteral("%1 %2 %3 ").arg(m_x).arg(m_y).arg(m_z) % c_xyzSpeedUnit.convertToQString(i18n) %
               u" | Rotation: " % QStringLiteral("%1 %2 %3 ").arg(m_pitch).arg(m_roll).arg(m_heading) %
               c_pbhAngleUnit.convertToQString(i18n) % u"/" % c_timeUnit.convertToQString(i18n);
    }

    QVariant CAircraftVelocity::propertyByIndex(CPropertyIndexRef index) const
    {
        SWIFT_VERIFY(index.isMyself());
        return QVariant::fromValue(*this);
    }

    void CAircraftVelocity::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        SWIFT_VERIFY(index.isMyself());
        *this = variant.value<CAircraftVelocity>();
    }

    int CAircraftVelocity::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftVelocity &compareValue) const
    {
        SWIFT_VERIFY(index.isMyself());
        return compare(*this, compareValue);
    }
}
