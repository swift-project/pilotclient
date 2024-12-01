// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/weather/cloudlayer.h"

#include <QHash>

using namespace swift::misc::aviation;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::weather, CCloudLayer)

namespace swift::misc::weather
{

    CCloudLayer::CCloudLayer(const CAltitude &base, const CAltitude &top, Coverage coverage) : m_base(base), m_top(top)
    {
        setCoverage(coverage);
    }

    CCloudLayer::CCloudLayer(const CAltitude &base, const CAltitude &top, double precipitationRate,
                             Precipitation precipitation, Clouds clouds, Coverage coverage)
        : m_base(base), m_top(top), m_precipitationRate(precipitationRate), m_precipitation(precipitation),
          m_clouds(clouds)
    {
        setCoverage(coverage);
    }

    void CCloudLayer::setCoverage(Coverage coverage) { m_coveragePercent = 100 * coverage / 4; }

    CCloudLayer::Coverage CCloudLayer::getCoverage() const
    {
        if (m_coveragePercent > 85) { return Overcast; }
        if (m_coveragePercent > 60 && m_coveragePercent <= 85) { return Broken; }
        if (m_coveragePercent > 30 && m_coveragePercent <= 60) { return Scattered; }
        if (m_coveragePercent > 10 && m_coveragePercent <= 30) { return Few; }
        return None;
    }

    QVariant CCloudLayer::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        switch (index.frontCasted<ColumnIndex>())
        {
        case IndexBase: return QVariant::fromValue(m_base);
        case IndexTop: return QVariant::fromValue(m_top);
        case IndexPrecipitationRate: return QVariant::fromValue(m_precipitationRate);
        case IndexPrecipitation: return QVariant::fromValue(m_precipitation);
        case IndexClouds: return QVariant::fromValue(m_clouds);
        case IndexCoveragePercent: return QVariant::fromValue(m_coveragePercent);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CCloudLayer::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CCloudLayer>();
            return;
        }
        switch (index.frontCasted<ColumnIndex>())
        {
        case IndexBase: setBase(variant.value<CAltitude>()); break;
        case IndexTop: setTop(variant.value<CAltitude>()); break;
        case IndexPrecipitationRate: setPrecipitationRate(variant.value<int>()); break;
        case IndexPrecipitation: setPrecipitation(variant.value<Precipitation>()); break;
        case IndexClouds: setClouds(variant.value<Clouds>()); break;
        case IndexCoveragePercent: setCoveragePercent(variant.value<int>()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    QString CCloudLayer::convertToQString(bool /** i18n **/) const
    {
        static const QHash<Coverage, QString> hash = {
            { None, "" }, { Few, "few" }, { Scattered, "scattered" }, { Broken, "broken" }, { Overcast, "overcast" }
        };

        return QStringLiteral("%1 from %2 to %3").arg(hash.value(getCoverage()), m_base.toQString(), m_top.toQString());
    }

} // namespace swift::misc::weather
