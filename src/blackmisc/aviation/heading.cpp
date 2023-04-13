/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/heading.h"
#include <QCoreApplication>

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc::Aviation
{
    QString CHeading::convertToQString(bool i18n) const
    {
        return i18n ?
                   QStringLiteral("%1 %2").arg(CAngle::convertToQString(i18n),
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
        Mixin::MetaType<CHeading>::registerMetadata();
        qRegisterMetaType<CHeading::ReferenceNorth>();
    }
} // namespace
