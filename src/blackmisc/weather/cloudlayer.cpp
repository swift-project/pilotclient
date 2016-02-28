/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

#include <tuple>
#include <QRegularExpression>

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Weather
    {

        CCloudLayer::CCloudLayer(BlackMisc::Aviation::CAltitude base,
                                 BlackMisc::Aviation::CAltitude ceiling,
                                 Coverage coverage) :
            m_ceiling(ceiling), m_base(base), m_coverage(coverage)
        { }

        CCloudLayer::CCloudLayer(BlackMisc::Aviation::CAltitude base,
                                 BlackMisc::Aviation::CAltitude ceiling,
                                 int precipitationRate,
                                 Precipitation precipitation,
                                 Clouds clouds,
                                 Coverage coverage) :
            m_ceiling(ceiling), m_base(base), m_precipitationRate(precipitationRate),
            m_precipitation(precipitation), m_clouds(clouds), m_coverage(coverage)
        { }

        CVariant CCloudLayer::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCeiling:
                return CVariant::fromValue(m_ceiling);
            case IndexCoverage:
                return CVariant::fromValue(m_coverage);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CCloudLayer::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CCloudLayer>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCeiling:
                setCeiling(variant.value<CAltitude>());
                break;
            case IndexCoverage:
                setCoverage(variant.value<Coverage>());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        QString CCloudLayer::convertToQString(bool /** i18n **/) const
        {
            static const QHash<Coverage, QString> hash =
            {
                { None, "" },
                { Few, "few" },
                { Scattered, "scattered" },
                { Broken, "broken" },
                { Overcast, "overcast" }
            };

            return QString("%1 in %2").arg(hash.value(m_coverage)).arg(m_ceiling.toQString());
        }

    } // namespace
} // namespace
