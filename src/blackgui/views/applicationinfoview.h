/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_APPLICATIONINFOVIEW_H
#define BLACKGUI_VIEWS_APPLICATIONINFOVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/applicationinfolistmodel.h"
#include "blackgui/blackguiexport.h"

class QWidget;

namespace BlackGui
{
    namespace Views
    {
        //! Network servers
        class BLACKGUI_EXPORT CApplicationInfoView : public CViewBase<Models::CApplicationInfoListModel, BlackMisc::CApplicationInfoList, BlackMisc::CApplicationInfo>
        {
        public:
            //! Constructor
            explicit CApplicationInfoView(QWidget *parent = nullptr);

            //! BlackMisc::CApplicationInfoList::otherSwiftVersionsFromDataDirectories
            int otherSwiftVersionsFromDataDirectories();
        };
    }
} // ns
#endif // guard
