/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_CLOUDLAYER_H
#define BLACKMISC_WEATHER_CLOUDLAYER_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Weather
    {
        /*!
         * Value object for a cloud layer
         */
        class BLACKMISC_EXPORT CCloudLayer : public CValueObject<CCloudLayer>
        {
        public:
            //! Cloud coverage
            enum Coverage
            {
                None,
                Few,
                Scattered,
                Broken,
                Overcast,
                CoverageUnknown
            };

            //! Cloud type
            enum Clouds
            {
                NoClouds,
                Cirrus,
                Stratus,
                Cumulus,
                Thunderstorm,
                CloudsUnknown
            };

            //! Precipitation Type
            enum Precipitation
            {
                NoPrecipitation,
                Rain,
                Snow,
                PrecipitationUnknown
            };

            //! Properties by index
            enum ColumnIndex
            {
                IndexBase = BlackMisc::CPropertyIndexRef::GlobalIndexCCloudLayer,
                IndexTop,
                IndexPrecipitationRate,
                IndexPrecipitation,
                IndexClouds,
                IndexCoveragePercent
            };

            //! Default constructor.
            CCloudLayer() = default;

            //! Constructor
            CCloudLayer(const BlackMisc::Aviation::CAltitude &base,
                        const BlackMisc::Aviation::CAltitude &top,
                        Coverage coverage);

            //! Constructor
            CCloudLayer(const BlackMisc::Aviation::CAltitude &base,
                        const BlackMisc::Aviation::CAltitude &top,
                        double precipitationRate,
                        Precipitation precipitation,
                        Clouds clouds,
                        Coverage coverage);

            //! Set base
            void setBase(const BlackMisc::Aviation::CAltitude &base) { m_base = base; }

            //! Get base
            BlackMisc::Aviation::CAltitude getBase() const { return m_base; }

            //! Set layer top
            void setTop(const BlackMisc::Aviation::CAltitude &top) { m_top = top; }

            //! Get layer top
            BlackMisc::Aviation::CAltitude getTop() const { return m_top; }

            //! Set precipitation rate in mm/h
            void setPrecipitationRate(double rate) { m_precipitationRate = rate; }

            //! Get precipitation rate in mm/h
            double getPrecipitationRate() const { return m_precipitationRate; }

            //! Set precipitation
            void setPrecipitation(Precipitation type) { m_precipitation = type; }

            //! Get precipitation
            Precipitation getPrecipitation() const { return m_precipitation; }

            //! Set cloud type
            void setClouds(Clouds type) { m_clouds = type; }

            //! Get cloud type
            Clouds getClouds() const { return m_clouds; }

            //! Set coverage
            void setCoverage(Coverage coverage);

            //! Get coverage
            Coverage getCoverage() const;

            //! Set coverage in %
            void setCoveragePercent(int coverage) { m_coveragePercent = coverage; }

            //! Get coverage in %
            int getCoveragePercent() const { return m_coveragePercent; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BlackMisc::Aviation::CAltitude m_base;
            BlackMisc::Aviation::CAltitude m_top;
            double m_precipitationRate = 0; //!< Unit mm/h
            Precipitation m_precipitation = NoPrecipitation;
            Clouds m_clouds = NoClouds;
            int m_coveragePercent;

            BLACK_METACLASS(
                CCloudLayer,
                BLACK_METAMEMBER(base),
                BLACK_METAMEMBER(top),
                BLACK_METAMEMBER(precipitationRate),
                BLACK_METAMEMBER(precipitation),
                BLACK_METAMEMBER(clouds),
                BLACK_METAMEMBER(coveragePercent)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer)
Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer::Coverage)
Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer::Clouds)
Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer::Precipitation)

#endif // guard
