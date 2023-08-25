// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_WEATHERDATAPLUGININFO_H
#define BLACKMISC_WEATHER_WEATHERDATAPLUGININFO_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Weather, CWeatherDataPluginInfo)

namespace BlackMisc::Weather
{
    //! Describing a weather data plugin
    class BLACKMISC_EXPORT CWeatherDataPluginInfo : public BlackMisc::CValueObject<CWeatherDataPluginInfo>
    {
    public:
        //! Default constructor
        CWeatherDataPluginInfo() = default;

        //! Constructor (used with unit tests)
        CWeatherDataPluginInfo(const QString &identifier, const QString &name,
                               const QString &description, bool valid);

        //! \copydoc BlackMisc::CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! Check if the provided plugin metadata is valid.
        //! Weather data plugin has to meet the following requirements:
        //!  * implements org.swift-project.blackcore.weatherdata;
        //!  * provides plugin name;
        bool isValid() const { return m_valid; }

        //! Identifier
        const QString &getIdentifier() const { return m_identifier; }

        //! Name
        const QString &getName() const { return m_name; }

        //! Description
        const QString &getDescription() const { return m_description; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_identifier;
        QString m_name;
        QString m_description;
        bool m_valid { false };

        BLACK_METACLASS(
            CWeatherDataPluginInfo,
            BLACK_METAMEMBER(identifier, 0, CaseInsensitiveComparison),
            BLACK_METAMEMBER(name, 0, DisabledForComparison | DisabledForHashing),
            BLACK_METAMEMBER(description, 0, DisabledForComparison | DisabledForHashing),
            BLACK_METAMEMBER(valid, 0, DisabledForComparison | DisabledForHashing)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Weather::CWeatherDataPluginInfo)

#endif // guard
