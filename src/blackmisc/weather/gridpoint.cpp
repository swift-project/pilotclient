// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/propertyindexref.h"

#include <QtDebug>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Weather, CGridPoint)

namespace BlackMisc::Weather
{
    CGridPoint::CGridPoint(const QString &identifier,
                           const ICoordinateGeodetic &position) : m_identifier(identifier),
                                                                  m_position(position)
    {}

    CGridPoint::CGridPoint(const QString &identifier,
                           const Geo::ICoordinateGeodetic &position,
                           const CCloudLayerList &cloudLayers,
                           const CTemperatureLayerList &temperatureLayers,
                           const CVisibilityLayerList &visibilityLayers,
                           const CWindLayerList &windLayers,
                           const CPressure &pressureAtMsl) : m_identifier(identifier),
                                                             m_position(position),
                                                             m_cloudLayers(cloudLayers),
                                                             m_temperatureLayers(temperatureLayers),
                                                             m_visibilityLayers(visibilityLayers),
                                                             m_windLayers(windLayers),
                                                             m_pressureAtMsl(pressureAtMsl)
    {}

    void CGridPoint::copyWeatherDataFrom(const CGridPoint &other)
    {
        setCloudLayers(other.getCloudLayers());
        setTemperatureLayers(other.getTemperatureLayers());
        setVisibilityLayers(other.getVisibilityLayers());
        setWindLayers(other.getWindLayers());
        setPressureAtMsl(other.getPressureAtMsl());
    }

    QVariant CGridPoint::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIdentifier:
            return QVariant::fromValue(m_identifier);
        case IndexPosition:
            return QVariant::fromValue(m_position);
        case IndexCloudLayers:
            return QVariant::fromValue(m_cloudLayers);
        case IndexTemperatureLayers:
            return QVariant::fromValue(m_temperatureLayers);
        case IndexWindLayers:
            return QVariant::fromValue(m_windLayers);
        case IndexPressureAtMsl:
            return QVariant::fromValue(m_pressureAtMsl);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CGridPoint::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CGridPoint>();
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIdentifier:
            setIdentifier(variant.value<QString>());
            break;
        case IndexPosition:
            setPosition(variant.value<CCoordinateGeodetic>());
            break;
        case IndexCloudLayers:
            setCloudLayers(variant.value<CCloudLayerList>());
            break;
        case IndexTemperatureLayers:
            setTemperatureLayers(variant.value<CTemperatureLayerList>());
            break;
        case IndexWindLayers:
            setWindLayers(variant.value<CWindLayerList>());
            break;
        case IndexPressureAtMsl:
            setPressureAtMsl(variant.value<CPressure>());
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }

    QString CGridPoint::convertToQString(bool /** i18n **/) const
    {
        qFatal("Not yet implemented!");
        return {};
    }

} // namespace
