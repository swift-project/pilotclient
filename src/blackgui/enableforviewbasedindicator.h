// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_ENABLEFORVIEWBASEDINDICATOR_H
#define BLACKGUI_ENABLEFORVIEWBASEDINDICATOR_H

#include "blackgui/blackguiexport.h"

namespace BlackGui::Views
{
    class CViewBaseNonTemplate;
}
namespace BlackGui
{
    //! Hand over indication task to an embedded view
    class BLACKGUI_EXPORT CEnableForViewBasedIndicator
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
        CEnableForViewBasedIndicator(Views::CViewBaseNonTemplate *viewWithIndicator = nullptr);

        //! Set the corresponding view
        void setViewWithIndicator(BlackGui::Views::CViewBaseNonTemplate *viewWithIndicator);

    private:
        Views::CViewBaseNonTemplate *m_viewWithIndicator = nullptr;
    };

} // namespace

#endif // guard
