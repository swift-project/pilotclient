//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/interpolation/interpolationstatus.h"

#include "misc/aviation/aircraftsituation.h"

using namespace swift::misc::aviation;

namespace swift::misc::simulation
{
    void CInterpolationStatus::setExtraInfo(const QString &info) { m_extraInfo = info; }

    void CInterpolationStatus::setInterpolatedAndCheckSituation(bool succeeded, const CAircraftSituation &situation)
    {
        m_isInterpolated = succeeded;
        this->checkIfValidSituation(situation);
    }

    void CInterpolationStatus::checkIfValidSituation(const CAircraftSituation &situation)
    {
        m_isValidSituation = !situation.isPositionOrAltitudeNull();
        if (!m_isValidSituation) { m_isValidSituation = false; }
    }

    bool CInterpolationStatus::hasValidInterpolatedSituation() const { return m_isInterpolated && m_isValidSituation; }

    QString CInterpolationStatus::toQString() const
    {
        return QStringLiteral("Interpolated: ") % boolToYesNo(m_isInterpolated) % QStringLiteral(" | situations: ") %
               QString::number(m_situations) % QStringLiteral(" | situation valid: ") %
               boolToYesNo(m_isValidSituation) % QStringLiteral(" | same: ") % boolToYesNo(m_isSameSituation) %
               (m_extraInfo.isEmpty() ? QString() : QStringLiteral(" info: ") % m_extraInfo);
    }
} // namespace swift::misc::simulation
