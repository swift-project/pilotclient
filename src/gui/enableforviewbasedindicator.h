// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_ENABLEFORVIEWBASEDINDICATOR_H
#define SWIFT_GUI_ENABLEFORVIEWBASEDINDICATOR_H

#include "gui/swiftguiexport.h"

namespace swift::gui::views
{
    class CViewBaseNonTemplate;
}
namespace swift::gui
{
    //! Hand over indication task to an embedded view
    class SWIFT_GUI_EXPORT CEnableForViewBasedIndicator
    {
    public:
        //! Enable loading indicator
        void enableLoadIndicator(bool enable);

        //! Showing load indicator?
        bool isShowingLoadIndicator() const;

        //! Show loading indicator
        int showLoadIndicator(int timeoutMs = -1);

        //! Hide loading indicator
        void hideLoadIndicator();

    protected:
        //! Constructor
        CEnableForViewBasedIndicator(views::CViewBaseNonTemplate *viewWithIndicator = nullptr);

        //! Set the corresponding view
        void setViewWithIndicator(swift::gui::views::CViewBaseNonTemplate *viewWithIndicator);

    private:
        views::CViewBaseNonTemplate *m_viewWithIndicator = nullptr;
    };

} // namespace swift::gui

#endif // guard
