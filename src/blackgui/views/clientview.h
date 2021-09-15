/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_CLIENTVIEW_H
#define BLACKGUI_VIEWS_CLIENTVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/clientlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/network/clientlist.h"

namespace BlackMisc::Network { class CClient; }
namespace BlackGui::Views
{
    //! Client view
    class BLACKGUI_EXPORT CClientView : public CViewBase<Models::CClientListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CClientView(QWidget *parent = nullptr);
    };
}
#endif // guard
