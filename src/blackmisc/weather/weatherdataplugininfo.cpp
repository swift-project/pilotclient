// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/weather/weatherdataplugininfo.h"

#include <QJsonValue>
#include <QtGlobal>

using namespace BlackMisc;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Weather, CWeatherDataPluginInfo)

namespace BlackMisc::Weather
{
    CWeatherDataPluginInfo::CWeatherDataPluginInfo(const QString &identifier, const QString &name, const QString &description, bool valid) : m_identifier(identifier), m_name(name), m_description(description), m_valid(valid)
    {}

    void CWeatherDataPluginInfo::convertFromJson(const QJsonObject &json)
    {
        if (json.contains("IID")) // comes from the plugin
        {
            if (!json.contains("MetaData")) { throw CJsonException("Missing 'MetaData'"); }

            // json data is already validated by CPluginManagerWeatherData
            CJsonScope scope("MetaData"); // for stack trace
            Q_UNUSED(scope);
            CValueObject::convertFromJson(json["MetaData"].toObject());
            m_valid = true;
        }
        else
        {
            CValueObject::convertFromJson(json);
        }
    }

    QString CWeatherDataPluginInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("%1 (%2)").arg(m_name, m_identifier);
    }

} // ns
