/* Copyright (C) 2018
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/columns.h"
#include "blackgui/models/interpolationsetupmodel.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackGui::Models
{
    CInterpolationSetupListModel::CInterpolationSetupListModel(QObject *parent) :
        CListModelCallsignObjects("CInterpolationSetupListModel", parent)
    {
        m_columns.addColumn(CColumn::standardValueObject("cs.", "callsign", CInterpolationAndRenderingSetupPerCallsign::IndexCallsign, CCallsign::IndexCallsignString));
        this->m_columns.addColumn(CColumn::standardString("interpolator",   CInterpolationAndRenderingSetupPerCallsign::IndexInterpolatorModeAsString));
        m_columns.addColumn(CColumn("parts", "aircraft parts",         CInterpolationAndRenderingSetupPerCallsign::IndexEnabledAircraftParts,   new CBoolIconFormatter("parts", "aircraft parts")));
        m_columns.addColumn(CColumn("send gnd.", "send gnd.",          CInterpolationAndRenderingSetupPerCallsign::IndexSendGndFlagToSimulator, new CBoolIconFormatter("send gnd.", "send gnd.")));
        m_columns.addColumn(CColumn("sc.os.", "fix scenery offset",    CInterpolationAndRenderingSetupPerCallsign::IndexFixSceneryOffset,       new CBoolIconFormatter("sc.os.", "fix scenery offset")));
        m_columns.addColumn(CColumn("full int.", "full interpolation", CInterpolationAndRenderingSetupPerCallsign::IndexForceFullInterpolation, new CBoolIconFormatter("full int.", "full interpolation")));
        m_columns.addColumn(CColumn("sim.dbg.", "simulator debug",     CInterpolationAndRenderingSetupPerCallsign::IndexSimulatorDebugMessages, new CBoolIconFormatter("sim.dbg.", "simulator debug")));
        m_columns.addColumn(CColumn("log.int.", "log.interpolation",   CInterpolationAndRenderingSetupPerCallsign::IndexLogInterpolation,       new CBoolIconFormatter("log.int.", "log.interpolation")));
        m_columns.addColumn(CColumn("gnd.pitch", "pitch on ground",    CInterpolationAndRenderingSetupPerCallsign::IndexPitchOnGround,          new CPhysiqalQuantiyFormatter<CAngleUnit, CAngle>(CAngleUnit::deg(), 1)));

        // default sort order
        this->setSortColumnByPropertyIndex(CCountry::IndexIsoCode);
        m_sortOrder = Qt::AscendingOrder;
    }
} // ns
