/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ENABLEFORVIEWBASEDINDICATOR_H
#define BLACKGUI_ENABLEFORVIEWBASEDINDICATOR_H

#include "blackgui/blackguiexport.h"

namespace BlackGui::Views { class CViewBaseNonTemplate; }
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
