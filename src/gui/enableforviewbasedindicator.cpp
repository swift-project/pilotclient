// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/enableforviewbasedindicator.h"

#include <QtGlobal>

#include "gui/views/viewbase.h"

using namespace swift::misc;
using namespace swift::gui;
using namespace swift::gui::views;

namespace swift::gui
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

    CEnableForViewBasedIndicator::CEnableForViewBasedIndicator(swift::gui::views::CViewBaseNonTemplate *viewWithIndicator)
        : m_viewWithIndicator(viewWithIndicator)
    {
        // void
    }

    void CEnableForViewBasedIndicator::setViewWithIndicator(swift::gui::views::CViewBaseNonTemplate *viewWithIndicator)
    {
        this->m_viewWithIndicator = viewWithIndicator;
    }
} // namespace swift::gui
