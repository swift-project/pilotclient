// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/temperaturelayerlistmodel.h"
#include "blackgui/models/columnformatters.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;

namespace BlackGui::Models
{

    //! Temperature in degrees
    class CTemperatureFormatter : public CPhysiqalQuantiyFormatter<CTemperatureUnit, CTemperature>
    {
    public:
        //! Constructor
        CTemperatureFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(CTemperatureUnit::C(), 0, alignment, withUnit, i18n) {}
    };

    //! Relative Humidity
    class CRelativeHumidityFormatter : public CDefaultFormatter
    {
    public:
        //! Constructor
        CRelativeHumidityFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

        //! \copydoc CDefaultFormatter::displayRole
        virtual CVariant displayRole(const CVariant &dataCVariant) const override
        {
            if (dataCVariant.canConvert<int>())
            {
                int rh = dataCVariant.value<int>();
                QString formattedString = QString::number(rh) + " %";
                return formattedString;
            }
            Q_ASSERT_X(false, "CRelativeHumidityFormatter", "no double value");
            return CVariant();
        }
    };

    CTemperatureLayerListModel::CTemperatureLayerListModel(QObject *parent) : CListModelBase("TemperatureLayerListModel", parent)
    {
        m_columns.addColumn(CColumn("level", CTemperatureLayer::IndexLevel, new CAltitudeFormatter()));
        m_columns.addColumn(CColumn("temperature", CTemperatureLayer::IndexTemperature, new CTemperatureFormatter()));
        m_columns.addColumn(CColumn("dew point", CTemperatureLayer::IndexDewPoint, new CTemperatureFormatter()));
        m_columns.addColumn(CColumn("relative humidity", CTemperatureLayer::IndexRelativeHumidity, new CRelativeHumidityFormatter()));

        // default sort order
        this->setSortColumnByPropertyIndex(CTemperatureLayer::IndexLevel);
        m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelTemperatureLayerList", "level");
        (void)QT_TRANSLATE_NOOP("ModelTemperatureLayerList", "temperature");
        (void)QT_TRANSLATE_NOOP("ModelTemperatureLayerList", "dew point");
        (void)QT_TRANSLATE_NOOP("ModelTemperatureLayerList", "relative humidity");
    }
} // ns
