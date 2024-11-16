// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "windlayerlistmodel.h"

#include <Qt>
#include <QtGlobal>

#include "gui/models/columnformatters.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::weather;

namespace swift::gui::models
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
} // namespace swift::gui::models
