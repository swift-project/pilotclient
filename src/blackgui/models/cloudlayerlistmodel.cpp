/* Copyright (C) 2016
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/cloudlayerlistmodel.h"
#include "blackgui/models/columnformatters.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;

namespace BlackGui::Models
{

    //! Relative Humidity
    class CPrecipitationRateFormatter : public CDefaultFormatter
    {
    public:
        //! Constructor
        CPrecipitationRateFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

        //! \copydoc CDefaultFormatter::displayRole
        virtual CVariant displayRole(const CVariant &dataCVariant) const override
        {
            if (dataCVariant.canConvert<double>())
            {
                double rate = dataCVariant.value<double>();
                QString formattedString = QString::number(rate) + " mm/h";
                return formattedString;
            }
            Q_ASSERT_X(false, "CPrecipitationRateFormatter", "no double value");
            return CVariant();
        }
    };

    //! Precipitation
    class CPrecipitationFormatter : public CDefaultFormatter
    {
    public:
        //! Constructor
        CPrecipitationFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

        //! \copydoc CDefaultFormatter::displayRole
        virtual CVariant displayRole(const CVariant &dataCVariant) const override
        {
            if (dataCVariant.canConvert<CCloudLayer::Precipitation>())
            {
                CCloudLayer::Precipitation precipitation = dataCVariant.value<CCloudLayer::Precipitation>();
                switch (precipitation)
                {
                case CCloudLayer::PrecipitationUnknown: return QString("Unknown");
                case CCloudLayer::NoPrecipitation: return QString("None");
                case CCloudLayer::Rain: return QString("Rain");
                case CCloudLayer::Snow: return QString("Snow");
                }
            }
            Q_ASSERT_X(false, "CPrecipitationFormatter", "no CCloudLayer::Precipitation value");
            return CVariant();
        }
    };

    //! Clouds
    class CCloudsFormatter : public CDefaultFormatter
    {
    public:
        //! Constructor
        CCloudsFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

        //! \copydoc CDefaultFormatter::displayRole
        virtual CVariant displayRole(const CVariant &dataCVariant) const override
        {
            if (dataCVariant.canConvert<CCloudLayer::Clouds>())
            {
                CCloudLayer::Clouds clouds = dataCVariant.value<CCloudLayer::Clouds>();
                switch (clouds)
                {
                case CCloudLayer::NoClouds: return QString("None");
                case CCloudLayer::Cirrus: return QString("Cirrus");
                case CCloudLayer::Stratus: return QString("Stratus");
                case CCloudLayer::Cumulus: return QString("Cumulus");
                case CCloudLayer::Thunderstorm: return QString("Thunderstorm");
                case CCloudLayer::CloudsUnknown: return QString("N/A");
                }
            }
            Q_ASSERT_X(false, "CCloudsFormatter", "no CCloudLayer::Clouds value");
            return CVariant();
        }
    };

    //! Relative Humidity
    class CCoverageFormatter : public CDefaultFormatter
    {
    public:
        //! Constructor
        CCoverageFormatter(int alignment = alignDefault()) : CDefaultFormatter(alignment, false, roleDisplay()) {}

        //! \copydoc CDefaultFormatter::displayRole
        virtual CVariant displayRole(const CVariant &dataCVariant) const override
        {
            if (dataCVariant.canConvert<int>())
            {
                int coverage = dataCVariant.value<int>();
                QString formattedString = QString::number(coverage) + " %";
                return formattedString;
            }
            Q_ASSERT_X(false, "CCoverageFormatter", "no int value");
            return CVariant();
        }
    };

    CCloudLayerListModel::CCloudLayerListModel(QObject *parent) :
        CListModelBase("CloudLayerListModel", parent)
    {
        this->m_columns.addColumn(CColumn("base", CCloudLayer::IndexBase, new CAltitudeFormatter()));
        this->m_columns.addColumn(CColumn("top", CCloudLayer::IndexTop, new CAltitudeFormatter()));
        this->m_columns.addColumn(CColumn("precipitation rate", CCloudLayer::IndexPrecipitationRate, new CPrecipitationRateFormatter()));
        this->m_columns.addColumn(CColumn("precipitation", CCloudLayer::IndexPrecipitation, new CPrecipitationFormatter()));
        this->m_columns.addColumn(CColumn("clouds", CCloudLayer::IndexClouds, new CCloudsFormatter()));
        this->m_columns.addColumn(CColumn("coverage", CCloudLayer::IndexCoveragePercent, new CCoverageFormatter()));

        // default sort order
        this->setSortColumnByPropertyIndex(CCloudLayer::IndexBase);
        this->m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelCloudLayerList", "base");
        (void)QT_TRANSLATE_NOOP("ModelCloudLayerList", "top");
        (void)QT_TRANSLATE_NOOP("ModelCloudLayerList", "precipitation rate");
        (void)QT_TRANSLATE_NOOP("ModelCloudLayerList", "precipitation");
        (void)QT_TRANSLATE_NOOP("ModelCloudLayerList", "clouds");
        (void)QT_TRANSLATE_NOOP("ModelCloudLayerList", "coverage");
    }
} // ns
