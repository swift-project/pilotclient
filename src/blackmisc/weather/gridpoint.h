/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_GRIDPOINT_H
#define BLACKMISC_WEATHER_GRIDPOINT_H

#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/visibilitylayerlist.h"
#include "blackmisc/weather/windlayerlist.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"
#include "blackmisc/metaclass.h"

#include <QMetaType>
#include <QString>

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
                IndexIdentifier = CPropertyIndex::GlobalIndexCGridPoint,
                IndexPosition,
                IndexCloudLayers,
                IndexTemperatureLayers,
                IndexWindLayers,
                IndexPressureAtMsl
            };

            //! Default constructor.
            CGridPoint() = default;

            //! Constructor
            CGridPoint(const QString &identifier,
                       const Geo::ICoordinateGeodetic &position);

            //! Constructor
            CGridPoint(const QString &identifier,
                       const Geo::ICoordinateGeodetic &position,
                       const CCloudLayerList &cloudLayers,
                       const CTemperatureLayerList &temperatureLayers,
                       const CVisibilityLayerList &visibilityLayers,
                       const CWindLayerList &windLayers,
                       const PhysicalQuantities::CPressure &pressureAtMsl);

            //! Set identifier
            void setIdentifier(const QString &identifier) { m_identifier = identifier; }

            //! Get identifier
            const QString &getIdentifier() const { return m_identifier; }

            //! Set position
            void setPosition(const Geo::CCoordinateGeodetic &position) { m_position = position; }

            //! Get position
            const Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

            //! Set cloud layers
            void setCloudLayers(const CCloudLayerList &cloudLayers) { m_cloudLayers = cloudLayers; }

            //! Get cloud layers
            const CCloudLayerList &getCloudLayers() const { return m_cloudLayers; }

            //! Set temperature layers
            void setTemperatureLayers(const CTemperatureLayerList &temperatureLayers) { m_temperatureLayers = temperatureLayers; }

            //! Get temperature layers
            const CTemperatureLayerList &getTemperatureLayers() const { return m_temperatureLayers; }

            //! Set visibility layers
            void setVisibilityLayers(const CVisibilityLayerList &visibilityLayers) { m_visibilityLayers = visibilityLayers; }

            //! Get visibility layers
            const CVisibilityLayerList &getVisibilityLayers() const { return m_visibilityLayers; }

            //! Set wind layers
            void setWindLayers(const CWindLayerList &windLayers) { m_windLayers = windLayers; }

            //! Get wind layers
            const CWindLayerList &getWindLayers() const { return m_windLayers; }

            //! Copies all weather data from other without modifying identifier and position.
            void copyWeatherDataFrom(const CGridPoint &other);

            //! Set pressure at mean sea level
            void setPressureAtMsl(const PhysicalQuantities::CPressure &pressure) { m_pressureAtMsl = pressure; }

            //! Get pressure at mean sea level
            const PhysicalQuantities::CPressure &getPressureAtMsl() const { return m_pressureAtMsl; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            QString m_identifier; //!< Identifier is intentionally string based. MSFS uses ICAO, but others don't.
            Geo::CCoordinateGeodetic m_position;
            CCloudLayerList m_cloudLayers;
            CTemperatureLayerList m_temperatureLayers;
            CVisibilityLayerList m_visibilityLayers;
            CWindLayerList m_windLayers;
            PhysicalQuantities::CPressure m_pressureAtMsl = { PhysicalQuantities::CPhysicalQuantitiesConstants::ISASeaLevelPressure() };

            BLACK_METACLASS(
                CGridPoint,
                BLACK_METAMEMBER(identifier),
                BLACK_METAMEMBER(position),
                BLACK_METAMEMBER(cloudLayers),
                BLACK_METAMEMBER(temperatureLayers),
                BLACK_METAMEMBER(visibilityLayers),
                BLACK_METAMEMBER(windLayers),
                BLACK_METAMEMBER(pressureAtMsl)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CGridPoint)

#endif // guard
