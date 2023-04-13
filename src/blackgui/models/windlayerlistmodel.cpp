/* Copyright (C) 2016
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "windlayerlistmodel.h"
#include "blackgui/models/columnformatters.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;

namespace BlackGui::Models
{
    CWindLayerListModel::CWindLayerListModel(QObject *parent) : CListModelBase("WindLayerListModel", parent)
    {
        m_columns.addColumn(CColumn("level", CWindLayer::IndexLevel, new CAltitudeFormatter()));
        m_columns.addColumn(CColumn("direction", CWindLayer::IndexDirection, new CAngleDegreeFormatter()));
        m_columns.addColumn(CColumn("speed", CWindLayer::IndexSpeed, new CSpeedKtsFormatter()));
        m_columns.addColumn(CColumn("gust speed", CWindLayer::IndexGustSpeed, new CSpeedKtsFormatter()));

        // default sort order
        this->setSortColumnByPropertyIndex(CWindLayer::IndexLevel);
        m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "level");
        (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "direction");
        (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "speed");
        (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "gust speed");
    }
} // ns
