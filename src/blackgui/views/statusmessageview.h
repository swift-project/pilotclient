/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_STATUSMESSAGEVIEW_H
#define BLACKGUI_STATUSMESSAGEVIEW_H

//! \file

#include "viewbase.h"
#include "../models/statusmessagelistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Status message view
        class CStatusMessageView : public CViewBase<Models::CStatusMessageListModel, BlackMisc::CStatusMessageList, BlackMisc::CStatusMessage>
        {

        public:
            //! Constructor
            explicit CStatusMessageView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
