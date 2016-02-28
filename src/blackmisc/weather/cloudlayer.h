/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_CLOUDLAYER_H
#define BLACKMISC_WEATHER_CLOUDLAYER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/aviation/altitude.h"

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
                Overcast
            };

            //! Cloud type
            enum Clouds
            {
                NoClouds,
                Cirrus,
                Stratus,
                Cumulus,
                Thunderstorm
            };

            //! Precipitation Type
            enum Precipitation
            {
                NoPrecipitation,
                Rain,
                Snow
            };

            //! Properties by index
            enum ColumnIndex
            {
                IndexCloudLayer = BlackMisc::CPropertyIndex::GlobalIndexCCloudLayer,
                IndexCeiling,
                IndexCoverage
            };

            //! Default constructor.
            CCloudLayer() = default;

            //! Constructor
            CCloudLayer(BlackMisc::Aviation::CAltitude base,
                        BlackMisc::Aviation::CAltitude ceiling,
                        Coverage coverage);

            //! Constructor
            CCloudLayer(BlackMisc::Aviation::CAltitude base,
                        BlackMisc::Aviation::CAltitude ceiling,
                        int precipitationRate,
                        Precipitation precipitation,
                        Clouds clouds,
                        Coverage coverage);

            //! Set base
            void setBase(const BlackMisc::Aviation::CAltitude &base) { m_base = base; }

            //! Get base
            BlackMisc::Aviation::CAltitude getBase() const { return m_base; }

            //! Set ceiling
            void setCeiling(BlackMisc::Aviation::CAltitude ceiling) { m_ceiling = ceiling; }

            //! Get ceiling
            BlackMisc::Aviation::CAltitude getCeiling() const { return m_ceiling; }

            //! Set precipitation rate
            void setPrecipitationRate(int rate) { m_precipitationRate = rate; }

            //! Get precipitation rate
            int getPrecipitationRate() const { return m_precipitationRate; }

            //! Set precipitation
            void setPrecipitation(Precipitation type) { m_precipitation = type; }

            //! Get precipitation
            Precipitation getPrecipitation() const { return m_precipitation; }

            //! Set cloud type
            void setClouds(Clouds type) { m_clouds = type; }

            //! Get cloud type
            Clouds getClouds() const { return m_clouds; }

            //! Set coverage
            void setCoverage(Coverage coverage) { m_coverage = coverage; }

            //! Get coverage
            Coverage getCoverage() const { return m_coverage; }

            //! Set coverage in %
            void setCoveragePercent(int coverage) { m_coveragePercent = coverage; }

            //! Get coverage in %
            int getCoveragePercent() const { return m_coveragePercent; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CCloudLayer)
            BlackMisc::Aviation::CAltitude m_base;
            BlackMisc::Aviation::CAltitude m_ceiling;
            int m_precipitationRate = 0;
            Precipitation m_precipitation = NoPrecipitation;
            Clouds m_clouds = NoClouds;
            Coverage m_coverage;
            int m_coveragePercent;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer)
Q_DECLARE_METATYPE(BlackMisc::Weather::CCloudLayer::Coverage)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Weather::CCloudLayer, (
                                   attr(o.m_base),
                                   attr(o.m_ceiling),
                                   attr(o.m_precipitationRate),
                                   attr(o.m_precipitation),
                                   attr(o.m_clouds),
                                   attr(o.m_coverage),
                                   attr(o.m_coveragePercent)
))

#endif // guard
