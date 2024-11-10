// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WEATHER_PRESENTWEATHER_H
#define SWIFT_MISC_WEATHER_PRESENTWEATHER_H

#include "misc/swiftmiscexport.h"
#include "misc/dictionary.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"

#include <QHash>
#include <QMetaType>
#include <QString>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::weather, CPresentWeather)

namespace swift::misc::weather
{
    /*!
     * Value object for present weather flags
     */
    class SWIFT_MISC_EXPORT CPresentWeather : public CValueObject<CPresentWeather>
    {
    public:
        //! Intensity
        enum Intensity
        {
            Light,
            Moderate,
            Heavy,
            InVincinity
        };

        //! Descriptor
        enum Descriptor
        {
            None,
            Shallow,
            Patches,
            Partial,
            Drifting,
            Blowing,
            Showers,
            Thunderstorm,
            Freezing,
        };

        //! Weather Phenomenon
        enum WeatherPhenomenon
        {
            NoPhenomena = 0,
            Drizzle = 1 << 0,
            Rain = 1 << 1,
            Snow = 1 << 2,
            SnowGrains = 1 << 3,
            IceCrystals = 1 << 4,
            IcePellets = 1 << 5,
            Hail = 1 << 6,
            SnowPellets = 1 << 7,
            Unknown = 1 << 8,
            Mist = 1 << 9,
            Fog = 1 << 10,
            Smoke = 1 << 11,
            VolcanicAsh = 1 << 12,
            Dust = 1 << 13,
            Sand = 1 << 14,
            Haze = 1 << 15,
            DustSandWhirls = 1 << 16,
            Squalls = 1 << 17,
            TornadoOrWaterspout = 1 << 18,
            FunnelCloud = 1 << 19,
            Sandstorm = 1 << 20,
            Duststorm = 1 << 21,
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexPresentWeather = swift::misc::CPropertyIndexRef::GlobalIndexCPresentWeather,
            IndexIntensity,
            IndexDescriptor,
            IndexWeatherPhenomena
        };

        //! \copydoc swift::misc::CValueObject::registerMetadata
        static void registerMetadata();

        //! Default constructor.
        CPresentWeather() = default;

        //! Constructor
        CPresentWeather(Intensity intensity, Descriptor descriptor, int weatherPhenomena);

        //! Set intensity
        void setIntensity(Intensity intensity) { m_intensity = intensity; }

        //! Get intensity
        Intensity getIntensity() const { return m_intensity; }

        //! Set descriptor
        void setDescriptor(Descriptor descriptor) { m_descriptor = descriptor; }

        //! Get descriptor
        Descriptor getDescriptor() const { return m_descriptor; }

        //! Set weather phenomena
        void setWeatherPhenomena(int phenomena) { m_weatherPhenomena = phenomena; }

        //! Get weather phenomenas
        int getWeatherPhenomena() const { return m_weatherPhenomena; }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        Intensity m_intensity = Moderate;
        Descriptor m_descriptor = None;
        int m_weatherPhenomena;

        BLACK_METACLASS(
            CPresentWeather,
            BLACK_METAMEMBER(intensity),
            BLACK_METAMEMBER(descriptor),
            BLACK_METAMEMBER(weatherPhenomena)
        );
    };

} // namespace

Q_DECLARE_METATYPE(swift::misc::weather::CPresentWeather)
Q_DECLARE_METATYPE(swift::misc::weather::CPresentWeather::Intensity)
Q_DECLARE_METATYPE(swift::misc::weather::CPresentWeather::Descriptor)

#endif // guard
