// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/columns.h"
#include "blackgui/models/interpolationsetupmodel.h"

#include <Qt>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;

namespace BlackGui::Models
{
    CInterpolationSetupListModel::CInterpolationSetupListModel(QObject *parent) : CListModelCallsignObjects("CInterpolationSetupListModel", parent)
    {
        m_columns.addColumn(CColumn::standardValueObject("cs.", "callsign", CInterpolationAndRenderingSetupPerCallsign::IndexCallsign, CCallsign::IndexCallsignString));
        this->m_columns.addColumn(CColumn::standardString("interpolator", CInterpolationAndRenderingSetupPerCallsign::IndexInterpolatorModeAsString));
        m_columns.addColumn(CColumn("parts", "aircraft parts", CInterpolationAndRenderingSetupPerCallsign::IndexEnabledAircraftParts, new CBoolIconFormatter("parts", "aircraft parts")));
        m_columns.addColumn(CColumn("send gnd.", "send gnd.", CInterpolationAndRenderingSetupPerCallsign::IndexSendGndFlagToSimulator, new CBoolIconFormatter("send gnd.", "send gnd.")));
        m_columns.addColumn(CColumn("sc.os.", "fix scenery offset", CInterpolationAndRenderingSetupPerCallsign::IndexFixSceneryOffset, new CBoolIconFormatter("sc.os.", "fix scenery offset")));
        m_columns.addColumn(CColumn("full int.", "full interpolation", CInterpolationAndRenderingSetupPerCallsign::IndexForceFullInterpolation, new CBoolIconFormatter("full int.", "full interpolation")));
        m_columns.addColumn(CColumn("sim.dbg.", "simulator debug", CInterpolationAndRenderingSetupPerCallsign::IndexSimulatorDebugMessages, new CBoolIconFormatter("sim.dbg.", "simulator debug")));
        m_columns.addColumn(CColumn("log.int.", "log.interpolation", CInterpolationAndRenderingSetupPerCallsign::IndexLogInterpolation, new CBoolIconFormatter("log.int.", "log.interpolation")));
        m_columns.addColumn(CColumn("gnd.pitch", "pitch on ground", CInterpolationAndRenderingSetupPerCallsign::IndexPitchOnGround, new CPhysiqalQuantiyFormatter<CAngleUnit, CAngle>(CAngleUnit::deg(), 1)));

        // default sort order
        this->setSortColumnByPropertyIndex(CCountry::IndexIsoCode);
        m_sortOrder = Qt::AscendingOrder;
    }
} // ns
