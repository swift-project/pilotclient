/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Weather
    {

        CGridPoint::CGridPoint(const Geo::CLatitude &latitude, const Geo::CLongitude longitude,
                               const CCloudLayerList &cloudLayers,
                               const CTemperatureLayerList &temperatureLayers,
                               const CVisibilityLayerList &visibilityLayers,
                               const CWindLayerList &windLayers) :
            m_latitude(latitude), m_longitude(longitude), m_cloudLayers(cloudLayers),
            m_temperatureLayers(temperatureLayers), m_visibilityLayers(visibilityLayers),
            m_windLayers(windLayers)
        { }

        CVariant CGridPoint::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLatitude:
                return CVariant::fromValue(m_latitude);
            case IndexLongitude:
                return CVariant::fromValue(m_longitude);
            case IndexCloudLayers:
                return CVariant::fromValue(m_cloudLayers);
            case IndexTemperatureLayers:
                return CVariant::fromValue(m_temperatureLayers);
            case IndexWindLayers:
                return CVariant::fromValue(m_windLayers);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CGridPoint::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CGridPoint>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLatitude:
                setLatitude(variant.value<CLatitude>());
                break;
            case IndexLongitude:
                setLongitude(variant.value<CLongitude>());
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
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        QString CGridPoint::convertToQString(bool /** i18n **/) const
        {
            qFatal("Not yet implemented!");
            return {};
        }

    } // namespace
} // namespace
