/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_GRIDPOINT_H
#define BLACKMISC_WEATHER_GRIDPOINT_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/windlayerlist.h"

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object for a cloud layer
         */
        class BLACKMISC_EXPORT CGridPoint : public CValueObject<CGridPoint>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexLatitude = BlackMisc::CPropertyIndex::GlobalIndexCGridPoint,
                IndexLongitude,
                IndexCloudLayers,
                IndexTemperatureLayers,
                IndexWindLayers
            };

            //! Default constructor.
            CGridPoint() = default;

            //! Constructor
            CGridPoint(const Geo::CLatitude &latitude, const Geo::CLongitude longitude,
                       const CCloudLayerList &cloudLayers,
                       const CTemperatureLayerList &temperatureLayers,
                       const CWindLayerList &windLayers);

            //! Set latitude
            void setLatitude(const Geo::CLatitude &latitude) { m_latitude = latitude; }

            //! Get latitude
            Geo::CLatitude getLatitude() const { return m_latitude; }

            //! Set longitude
            void setLongitude(const Geo::CLongitude &longitude) { m_longitude = longitude; }

            //! Get longitude
            Geo::CLongitude getLongitude() const { return m_longitude; }

            //! Set cloud layers
            void setCloudLayers(const CCloudLayerList &cloudLayers) { m_cloudLayers = cloudLayers; }

            //! Get cloud layers
            CCloudLayerList getCloudLayers() const { return m_cloudLayers; }

            //! Set temperature layers
            void setTemperatureLayers(const CTemperatureLayerList &temperatureLayers) { m_temperatureLayers = temperatureLayers; }

            //! Get temperature layers
            CTemperatureLayerList getTemperatureLayers() const { return m_temperatureLayers; }

            //! Set wind layers
            void setWindLayers(const CWindLayerList &windLayers) { m_windLayers = windLayers; }

            //! Get wind layers
            CWindLayerList getWindLayers() const { return m_windLayers; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CGridPoint)
            Geo::CLatitude m_latitude;
            Geo::CLongitude m_longitude;
            CCloudLayerList m_cloudLayers;
            CTemperatureLayerList m_temperatureLayers;
            CWindLayerList m_windLayers;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CGridPoint)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Weather::CGridPoint, (
                                   attr(o.m_latitude),
                                   attr(o.m_longitude),
                                   attr(o.m_cloudLayers),
                                   attr(o.m_temperatureLayers),
                                   attr(o.m_windLayers)
))

#endif // guard
