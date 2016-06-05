/* Copyright (C) 2016
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/windlayerlistmodel.h"
#include "blackgui/models/columnformatters.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;

namespace BlackGui
{
    namespace Models
    {

        //! Speed
        class CSpeedFormatter : public CPhysiqalQuantiyFormatter<BlackMisc::PhysicalQuantities::CSpeedUnit, BlackMisc::PhysicalQuantities::CSpeed>
        {
        public:
            //! Constructor
            CSpeedFormatter(int alignment = alignRightVCenter(), bool withUnit = true, bool i18n = true) : CPhysiqalQuantiyFormatter(BlackMisc::PhysicalQuantities::CSpeedUnit::kts(), 0, alignment, withUnit, i18n) {}

            //! \copydoc CDefaultFormatter::displayRole
            virtual CVariant displayRole(const CVariant &dataCVariant) const override
            {
                // special treatment for some cases
                BlackMisc::PhysicalQuantities::CSpeed s = dataCVariant.value<BlackMisc::PhysicalQuantities::CSpeed>();
                if (!s.isNull() && (s.isPositiveWithEpsilonConsidered() || s.isZeroEpsilonConsidered()))
                {
                    return CPhysiqalQuantiyFormatter::displayRole(dataCVariant);
                }
                else
                {
                    return "";
                }
            }
        };

        CWindLayerListModel::CWindLayerListModel(QObject *parent) :
            CListModelBase("WindLayerListModel", parent)
        {
            this->m_columns.addColumn(CColumn("level", CWindLayer::IndexLevel, new CAltitudeFormatter()));
            this->m_columns.addColumn(CColumn("direction", CWindLayer::IndexDirection, new CAngleDegreeFormatter()));
            this->m_columns.addColumn(CColumn("speed", CWindLayer::IndexSpeed, new CSpeedFormatter()));
            this->m_columns.addColumn(CColumn("gust speed", CWindLayer::IndexGustSpeed, new CSpeedFormatter()));

            // default sort order
            this->setSortColumnByPropertyIndex(CWindLayer::IndexLevel);
            this->m_sortOrder = Qt::AscendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "level");
            (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "direction");
            (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "speed");
            (void)QT_TRANSLATE_NOOP("ModelWindLayerList", "gust speed");
        }
    } // ns
} // ns
