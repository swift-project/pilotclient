// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/views/viewbase.h"

#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackGui::Views;

namespace BlackGui
{
    void CEnableForViewBasedIndicator::enableLoadIndicator(bool enable)
    {
        Q_ASSERT_X(m_viewWithIndicator, Q_FUNC_INFO, "Missing view to display indicator");
        if (m_viewWithIndicator) { m_viewWithIndicator->enableLoadIndicator(enable); }
    }

    bool CEnableForViewBasedIndicator::isShowingLoadIndicator() const
    {
        Q_ASSERT_X(m_viewWithIndicator, Q_FUNC_INFO, "Missing view to display indicator");
        if (m_viewWithIndicator) { return m_viewWithIndicator->isShowingLoadIndicator(); }
        return false;
    }

    int CEnableForViewBasedIndicator::showLoadIndicator(int timeoutMs)
    {
        Q_ASSERT_X(m_viewWithIndicator, Q_FUNC_INFO, "Missing view to display indicator");
        if (m_viewWithIndicator) { return m_viewWithIndicator->showLoadIndicatorWithTimeout(timeoutMs); }
        return -1;
    }

    void CEnableForViewBasedIndicator::hideLoadIndicator()
    {
        Q_ASSERT_X(m_viewWithIndicator, Q_FUNC_INFO, "Missing view to display indicator");
        if (m_viewWithIndicator) { m_viewWithIndicator->hideLoadIndicator(); }
    }

    CEnableForViewBasedIndicator::CEnableForViewBasedIndicator(BlackGui::Views::CViewBaseNonTemplate *viewWithIndicator)
        : m_viewWithIndicator(viewWithIndicator)
    {
        // void
    }

    void CEnableForViewBasedIndicator::setViewWithIndicator(BlackGui::Views::CViewBaseNonTemplate *viewWithIndicator)
    {
        this->m_viewWithIndicator = viewWithIndicator;
    }
} // namespace
