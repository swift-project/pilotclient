/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/heading.h"

#include <QCoreApplication>

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc
{
    namespace Aviation
    {
        QString CHeading::convertToQString(bool i18n) const
        {
            static const QString s("%1 %2");
            if (i18n)
            {
                return s.arg(CAngle::convertToQString(i18n),
                             this->isMagneticHeading() ?
                             QCoreApplication::translate("Aviation", "magnetic") :
                             QCoreApplication::translate("Aviation", "true"));
            }
            else
            {
                return s.arg(CAngle::convertToQString(i18n),
                             this->isMagneticHeading() ? "magnetic" : "true");
            }
        }

        void CHeading::registerMetadata()
        {
            Mixin::MetaType<CHeading>::registerMetadata();
            qRegisterMetaType<CHeading::ReferenceNorth>();
        }
    } // namespace
} // namespace
