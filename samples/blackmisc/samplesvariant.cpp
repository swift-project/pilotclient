/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesvariant.h"
#include "blackmisc/variant.h"
#include "blackmisc/pqallquantities.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QDebug>
#include <QMetaType>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMiscTest
{

    int CSamplesVariant::samples()
    {
        BlackMisc::registerMetadata();

        CAngle a1(30.0, CAngleUnit::deg());
        CVariant cva = a1.toCVariant();
        qDebug() << a1 << cva.userType();

        CHeading h1(45, CHeading::True, CAngleUnit::deg());
        CHeading h2(60, CHeading::True, CAngleUnit::deg());

        CVariant cvh = h1.toCVariant();
        qDebug() << h1 << cvh.userType();

        qDebug() << cva << cvh; // CVariant knows how to stringify the contained value object

        CAngle *ap_heading = &h1; // angle actually heading
        CAngle *ap_angle = &a1;   // angle really heading
        qDebug() << (*ap_heading) << ap_heading->toCVariant().userType();
        qDebug() << (*ap_angle) << ap_angle->toCVariant().userType();

        // This works, because ap is actually heading
        ap_heading->convertFromCVariant(h2.toCVariant());
        qDebug() << (*ap_heading) << ap_heading->toCVariant().userType();

        // This works, angle from variant angle
        ap_angle->convertFromCVariant(a1.toCVariant());
        qDebug() << (*ap_angle) << ap_angle->toCVariant().userType();

        // This gives me an unwanted(!) assert, canConvert is not smart enough to detect upcasting
        // because CValueObjects are not QObjects
        ap_angle->convertFromCVariant(h2.toCVariant());
        qDebug() << (*ap_angle) << ap_angle->toCVariant().userType();

        // This gives me the intended assert, because I assign angle to heading
        ap_heading->convertFromCVariant(a1.toCVariant());
        qDebug() << (*ap_heading) << ap_heading->toCVariant().userType();

        return 0;
    }

} // namespace
