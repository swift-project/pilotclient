// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_CLOUDLAYER_H
#define SWIFT_MISC_WEATHER_CLOUDLAYER_H

#include <QMetaType>
#include <QString>
#include <QVariant>

#include "misc/aviation/altitude.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::weather, CCloudLayer)

namespace swift::misc::weather
{
    /*!
     * Value object for a cloud layer
     */
    class SWIFT_MISC_EXPORT CCloudLayer : public CValueObject<CCloudLayer>
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
            IndexBase = swift::misc::CPropertyIndexRef::GlobalIndexCCloudLayer,
            IndexTop,
            IndexPrecipitationRate,
            IndexPrecipitation,
            IndexClouds,
            IndexCoveragePercent
        };

        //! Default constructor.
        CCloudLayer() = default;

        //! Constructor
        CCloudLayer(const swift::misc::aviation::CAltitude &base, const swift::misc::aviation::CAltitude &top,
                    Coverage coverage);

        //! Constructor
        CCloudLayer(const swift::misc::aviation::CAltitude &base, const swift::misc::aviation::CAltitude &top,
                    double precipitationRate, Precipitation precipitation, Clouds clouds, Coverage coverage);

        //! Set base
        void setBase(const swift::misc::aviation::CAltitude &base) { m_base = base; }

        //! Get base
        swift::misc::aviation::CAltitude getBase() const { return m_base; }

        //! Set layer top
        void setTop(const swift::misc::aviation::CAltitude &top) { m_top = top; }

        //! Get layer top
        swift::misc::aviation::CAltitude getTop() const { return m_top; }

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

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        swift::misc::aviation::CAltitude m_base;
        swift::misc::aviation::CAltitude m_top;
        double m_precipitationRate = 0; //!< Unit mm/h
        Precipitation m_precipitation = NoPrecipitation;
        Clouds m_clouds = NoClouds;
        int m_coveragePercent;

        SWIFT_METACLASS(
            CCloudLayer,
            SWIFT_METAMEMBER(base),
            SWIFT_METAMEMBER(top),
            SWIFT_METAMEMBER(precipitationRate),
            SWIFT_METAMEMBER(precipitation),
            SWIFT_METAMEMBER(clouds),
            SWIFT_METAMEMBER(coveragePercent));
    };
} // namespace swift::misc::weather

Q_DECLARE_METATYPE(swift::misc::weather::CCloudLayer)
Q_DECLARE_METATYPE(swift::misc::weather::CCloudLayer::Coverage)
Q_DECLARE_METATYPE(swift::misc::weather::CCloudLayer::Clouds)
Q_DECLARE_METATYPE(swift::misc::weather::CCloudLayer::Precipitation)

#endif // guard
