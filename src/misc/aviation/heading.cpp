// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/heading.h"

#include <QCoreApplication>

using swift::misc::physical_quantities::CAngle;
using swift::misc::physical_quantities::CAngleUnit;

namespace swift::misc::aviation
{
    QString CHeading::convertToQString(bool i18n) const
    {
        return i18n ? QStringLiteral("%1 %2").arg(CAngle::convertToQString(i18n),
                                                  this->isMagneticHeading() ?
                                                      QCoreApplication::translate("Aviation", "magnetic") :
                                                      QCoreApplication::translate("Aviation", "true")) :
                      QStringLiteral("%1 %2").arg(CAngle::convertToQString(i18n),
                                                  this->isMagneticHeading() ? "magnetic" : "true");
    }

    void CHeading::normalizeTo360Degrees()
    {
        const double v = normalizeDegrees360(this->value(CAngleUnit::deg()));
        const CAngleUnit u = this->getUnit();
        *this = CHeading(v, this->getReferenceNorth(), CAngleUnit::deg());
        this->switchUnit(u);
    }

    void CHeading::normalizeToPlusMinus180Degrees()
    {
        const double v = normalizeDegrees180(this->value(CAngleUnit::deg()));
        const CAngleUnit u = this->getUnit();
        *this = CHeading(v, this->getReferenceNorth(), CAngleUnit::deg());
        this->switchUnit(u);
    }

    CHeading CHeading::normalizedToPlusMinus180Degrees() const
    {
        CHeading copy(*this);
        copy.normalizeToPlusMinus180Degrees();
        return copy;
    }

    CHeading CHeading::normalizedTo360Degrees() const
    {
        CHeading copy(*this);
        copy.normalizeTo360Degrees();
        return copy;
    }

    void CHeading::registerMetadata()
    {
        mixin::MetaType<CHeading>::registerMetadata();
        qRegisterMetaType<CHeading::ReferenceNorth>();
    }
} // namespace swift::misc::aviation
