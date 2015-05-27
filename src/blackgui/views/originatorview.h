/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ORIGINATORVIEW_H
#define BLACKGUI_ORIGINATORVIEW_H

#include "blackgui/blackguiexport.h"
#include "viewbase.h"
#include "../models/originatorlistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Originator servers
        class BLACKGUI_EXPORT COriginatorView : public CViewBase<Models::COriginatorListModel, BlackMisc::COriginatorList, BlackMisc::COriginator>
        {

        public:

            //! Constructor
            explicit COriginatorView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
